#include <SDL.h>
#include <cmath>
#include <glm/glm.hpp>
#include <iostream>
#include <map>
#include <set>
#include <glm/gtc/constants.hpp>
#include <csignal>

#include "SDLauxiliary.hpp"
#include "interpolation.hpp"
#include "models/cornell_box.hpp"
#include "triangle.hpp"
#include "debug.hpp"

using namespace std;
using namespace cg;
using glm::vec2;
using glm::vec3;
using glm::ivec2;
using glm::mat3;

/* ----------------------------------------------------------------------------*/
/* GLOBAL VARIABLES */

#define MOUSE_CONTROLS_ON 0

const int SCREEN_WIDTH = 500;
const int SCREEN_HEIGHT = 500;
const float WORLD_WIDTH = 2;
const float WORLD_HEIGHT = 2;
const float WORLD_DEPTH = 2;
const float FOCAL_LENGTH = 2;

static const float TRANSLATION_STEP_SIZE = 0.01;
static const float ROTATION_STEP_SIZE = 0.005;

vector<float> screen_pixel_centres_y(SCREEN_HEIGHT);
vector<float> screen_pixel_centres_x(SCREEN_WIDTH);

float depthBuffer[SCREEN_HEIGHT][SCREEN_WIDTH];

struct Pixel {
  int x;
  int y;
  float zinv;
  vec3 pos3d;
};

SDL_Surface *screen;
int TIME;
vector<Triangle> triangles;
vec3 currentColor;

// Adding a tiny amount of camera rotation fixes little black spots that appear
// at the intersection of triangles
// on the right wall
float YAW = 0.0001;
float PITCH = 0;
float ROLL = 0;
vec3 CAMERA_CENTRE(0, 0, -3.001);

mat3 CAMERA_ROTATION;
mat3 CAMERA_ROTATION_X;
mat3 CAMERA_ROTATION_Y;
mat3 CAMERA_ROTATION_Z;

vec3 LIGHT_POSITION(0, -0.5, -0.7);
vec3 LIGHT_POWER = 14.f * vec3(1, 1, 1);
vec3 INDIRECT_LIGHT_POWER_PER_AREA = 0.5f * vec3(1, 1, 1);

vec3 currentNormal;
float currentReflectance;

/* ----------------------------------------------------------------------------*/
/* FUNCTIONS */

void Update();
void Draw();

void updateCameraRotation();

void updateCameraParameters(const Uint8 *keystate);

void updateCameraPosition(const Uint8 *keystate);

void updateCameraRotation(const Uint8 *keystate);

void updateLightPosition(const Uint8 *keystate);

float computeRenderTime();

void calculateScreenPixelCentres();

void vertexShader(const Vertex &v, Pixel &p);

void constructPixelLine(Pixel start, Pixel end, vector<Pixel> &line);

void drawLineSDL(SDL_Surface *surface, Pixel a, Pixel b, vec3 color);

void drawPolygonEdges(const vector<vec3> &vertices);

void interpolate(Pixel a, Pixel b, vector<Pixel> &result);

void computeRows(const vector<Pixel> &vertexPixels, vector<Pixel> &leftPixels,
                 vector<Pixel> &rightPixels);

void drawRows(const vector<Pixel> &leftPixels,
              const vector<Pixel> &rightPixels);

void drawPolygon(const vector<Vertex> &vertices);

void pixelShader(const Pixel &pixel);

void computeVertexNormals(vector<Triangle> &triangles);

void updateVertexNormal(const set<Triangle *> &triangles, const Vertex &vertex,
                        vec3 normal);

void setAdd(vector<Triangle> &triangles, Triangle &triangle);

int main(int argc, char *argv[]) {
  screen = InitializeSDL(SCREEN_WIDTH, SCREEN_HEIGHT, false);
  TIME = SDL_GetTicks(); // Set start value for timer.
  signal(SIGINT, SIG_DFL);

  LoadTestModel(triangles);
  calculateScreenPixelCentres();
  computeVertexNormals(triangles);

  while (NoQuitMessageSDL()) {
    Update();
    Draw();
  }

  return 0;
}

void computeVertexNormals(vector<Triangle> &triangles) {
  map<Vertex *, set<Triangle *>> vertexToTrianglesSharingVertex;

  for (auto &triangle : triangles) {
    vertexToTrianglesSharingVertex[&triangle.v0].insert(&triangle);
    vertexToTrianglesSharingVertex[&triangle.v1].insert(&triangle);
    vertexToTrianglesSharingVertex[&triangle.v2].insert(&triangle);
  }

  for (auto &p : vertexToTrianglesSharingVertex) {
    const Vertex *vertex = p.first;
    const set<Triangle *> &trianglesSharingVertex = p.second;
    vec3 normal = {0, 0, 0};
    for (auto &triangle : p.second) {
      normal += triangle->normal;
    }
    normal /= p.second.size();

    updateVertexNormal(trianglesSharingVertex, *vertex, normal);
  }
}

void updateVertexNormal(const set<Triangle *> &triangles, const Vertex &vertex,
                        vec3 normal) {
  for (auto &triangle : triangles) {
    if (triangle->v0 == vertex) {
      triangle->v0.normal = normal;
    }
    if (triangle->v1 == vertex) {
      triangle->v1.normal = normal;
    }
    if (triangle->v2 == vertex) {
      triangle->v2.normal = normal;
    }
  }
}

void Update() {
  cout << "Render time: " << computeRenderTime() << " ms." << endl;
  Uint8 *keystate = SDL_GetKeyState(0);
  updateCameraParameters(keystate);
  updateLightPosition(keystate);
  for (int y = 0; y < SCREEN_HEIGHT; ++y) {
    for (int x = 0; x < SCREEN_WIDTH; ++x) {
      depthBuffer[y][x] = 0;
    }
  }
}

void Draw() {
  SDL_FillRect(screen, 0, 0);

  if (SDL_MUSTLOCK(screen)) {
    SDL_LockSurface(screen);
  }

  updateCameraRotation();

  for (uint i = 0; i < triangles.size(); ++i) {
    currentColor = triangles[i].color;
    currentNormal = triangles[i].normal;
    // NOTE: We assume that the reflectance is constant over the triangle.
    currentReflectance = 1;
    vector<Vertex> vertices(3);
    vertices[0] = triangles[i].v0;
    vertices[1] = triangles[i].v1;
    vertices[2] = triangles[i].v2;
    drawPolygon(vertices);
  }

  if (SDL_MUSTLOCK(screen)) {
    SDL_UnlockSurface(screen);
  }

  SDL_UpdateRect(screen, 0, 0, 0, 0);
}

void interpolate(Pixel start, Pixel end, vector<Pixel> &result) {
  int N = result.size();
  float x_step_size = (end.x - start.x) / float(max(N - 1, 1));
  float y_step_size = (end.y - start.y) / float(max(N - 1, 1));
  float zinv_step_size = (end.zinv - start.zinv) / float(max(N - 1, 1));
  //Interpolating pos3d linearly with the position divided by the z value.
  vec3 pos3d_step_size = (end.pos3d * end.zinv - start.pos3d * start.zinv) / float(max(N - 1, 1));
  for (size_t i = 0; i < result.size(); i++) {
    result[i].x = glm::round(start.x + x_step_size * i);
    result[i].y = glm::round(start.y + y_step_size * i);
    result[i].zinv = start.zinv + zinv_step_size * i;
    result[i].pos3d = (start.pos3d * start.zinv + pos3d_step_size * float(i)) / result[i].zinv;
  }
}

void drawPolygon(const vector<Vertex> &vertices) {
  int V = vertices.size();
  vector<Pixel> vertexPixels(V);
  for (int i = 0; i < V; ++i) {
    vertexShader(vertices[i], vertexPixels[i]);
  }
  vector<Pixel> leftPixels;
  vector<Pixel> rightPixels;
  computeRows(vertexPixels, leftPixels, rightPixels);
  drawRows(leftPixels, rightPixels);
}

void drawRows(const vector<Pixel> &leftPixels,
              const vector<Pixel> &rightPixels) {
  for (uint i = 0; i < leftPixels.size(); i++) {
    Pixel leftPixel = leftPixels[i];
    Pixel rightPixel = rightPixels[i];
    int n = rightPixel.x - leftPixel.x + 1;
    vector<Pixel> line(n);
    interpolate(leftPixel, rightPixel, line);
    for (auto pixel : line) {
      if (pixel.y < SCREEN_HEIGHT && pixel.y >= 0 && pixel.x < SCREEN_WIDTH &&
          pixel.x >= 0) {
        pixelShader(pixel);
      }
    }
  }
}

void pixelShader(const Pixel &pixel) {
  // Vector from surface point to the light source
  vec3 surface_to_light = LIGHT_POSITION - pixel.pos3d;

  // Compute illumination of vertex
  float scale = (4 * glm::pi<float>() * length(surface_to_light) *
                 length(surface_to_light));
  vec3 direct_illumination =
      (LIGHT_POWER *
       max(dot(normalize(surface_to_light), normalize(currentNormal)), 0.0f)) /
      scale;

  vec3 illumination = currentReflectance *
                      (direct_illumination + INDIRECT_LIGHT_POWER_PER_AREA);

  int x = pixel.x;
  int y = pixel.y;
  if (pixel.zinv > depthBuffer[y][x]) {
    depthBuffer[y][x] = pixel.zinv;
    PutPixelSDL(screen, x, y, illumination * currentColor);
  }
}

void computeRows(const vector<Pixel> &vertexPixels, vector<Pixel> &leftPixels,
                 vector<Pixel> &rightPixels) {
  int max_y = vertexPixels[0].y;
  int min_y = vertexPixels[0].y;
  for (auto vertexPixel : vertexPixels) {
    if (vertexPixel.y < min_y) {
      min_y = vertexPixel.y;
    } else if (vertexPixel.y > max_y) {
      max_y = vertexPixel.y;
    }
  }
  uint numRowsOccupied = (uint)(max_y - min_y + 1);

  leftPixels.resize(numRowsOccupied);
  rightPixels.resize(numRowsOccupied);

  for (uint i = 0; i < numRowsOccupied; ++i) {
    leftPixels[i].x = +numeric_limits<int>::max();
    rightPixels[i].x = -numeric_limits<int>::max();
  }

  vector<vector<Pixel>> edges(3);
  constructPixelLine(vertexPixels[0], vertexPixels[1], edges[0]);
  constructPixelLine(vertexPixels[1], vertexPixels[2], edges[1]);
  constructPixelLine(vertexPixels[0], vertexPixels[2], edges[2]);

  // Find leftmost and rightmost pixels of the polygon for each row

  for (auto edge : edges) {
    for (auto pixel : edge) {
      int relative_y = pixel.y - min_y;
      assert(relative_y >= 0);
      assert(relative_y < leftPixels.size());
      assert(relative_y < rightPixels.size());
      if (pixel.x < leftPixels[relative_y].x) {
        leftPixels[relative_y] = pixel;
      }
      if (pixel.x > rightPixels[relative_y].x) {
        rightPixels[relative_y] = pixel;
      }
    }
  }
}
void drawLineSDL(SDL_Surface *surface, Pixel a, Pixel b, vec3 color) {
  vector<Pixel> edge;
  constructPixelLine(a, b, edge);
  for (auto pixel : edge) {
    PutPixelSDL(surface, pixel.x, pixel.y, color);
  }
}

void constructPixelLine(Pixel start, Pixel end, vector<Pixel> &line) {
  ivec2 delta = glm::abs(ivec2(start.x, start.y) - ivec2(end.x, end.y));

  uint pixel_count = (uint)glm::max(delta.x, delta.y) + 1;

  line.resize(pixel_count);
  interpolate(start, end, line);
}

void vertexShader(const Vertex &world_point, Pixel &pixel) {
  vec3 point = (world_point.position - CAMERA_CENTRE) * CAMERA_ROTATION;

  // Bad things will probably happen if z is zero
  assert(point.z != 0);
  pixel.zinv = 1 / point.z;

  // OPTIMISATION NOTE: * pixel.zinv == * 1 / point.z
  pixel.x =
      (int)((SCREEN_WIDTH / WORLD_WIDTH) * FOCAL_LENGTH * point.x * pixel.zinv +
            SCREEN_WIDTH / 2);
  pixel.y = (int)((SCREEN_HEIGHT / WORLD_HEIGHT) * FOCAL_LENGTH * point.y *
                      pixel.zinv +
                  SCREEN_HEIGHT / 2);

  pixel.pos3d = world_point.position;
}

void calculateScreenPixelCentres() {
  vec3 topLeft = vec3(
      (-WORLD_WIDTH / 2) + (WORLD_WIDTH / (2 * SCREEN_WIDTH)),
      (-WORLD_HEIGHT / 2) + (WORLD_HEIGHT / (2 * SCREEN_HEIGHT)), FOCAL_LENGTH);

  vec3 bottomRight = vec3(
      (WORLD_WIDTH / 2) - (WORLD_WIDTH / (2 * SCREEN_WIDTH)),
      (WORLD_HEIGHT / 2) - (WORLD_HEIGHT / (2 * SCREEN_HEIGHT)), FOCAL_LENGTH);

  vec3 bottomLeft = vec3(topLeft.x, bottomRight.y, FOCAL_LENGTH);
  vec3 topRight = vec3(bottomRight.x, topLeft.y, FOCAL_LENGTH);
  interpolate(topLeft.y, bottomLeft.y, screen_pixel_centres_y);
  interpolate(topLeft.x, topRight.x, screen_pixel_centres_x);
}

float computeRenderTime() {
  int t2 = SDL_GetTicks();
  float dt = float(t2 - TIME);
  TIME = t2;
  return dt;
}

void updateCameraParameters(const Uint8 *keystate) {
  updateCameraPosition(keystate);
  if (MOUSE_CONTROLS_ON) {
    updateCameraRotation(keystate);
  }
}

void updateCameraRotation(const Uint8 *keystate) {
  int x;
  int y;
  SDL_GetRelativeMouseState(&x, &y);
  PITCH += -y / (SCREEN_HEIGHT * 1.0);
  YAW += x / (SCREEN_WIDTH * 1.0);
}

void updateCameraPosition(const Uint8 *keystate) {
  vec3 right(CAMERA_ROTATION[0][0], CAMERA_ROTATION[0][1],
             CAMERA_ROTATION[0][2]);
  vec3 down(CAMERA_ROTATION[1][0], CAMERA_ROTATION[1][1],
            CAMERA_ROTATION[1][2]);
  vec3 forward(CAMERA_ROTATION[2][0], CAMERA_ROTATION[2][1],
               CAMERA_ROTATION[2][2]);
  if (keystate[SDLK_q]) {
    CAMERA_CENTRE += down * TRANSLATION_STEP_SIZE;
  }
  if (keystate[SDLK_e]) {
    CAMERA_CENTRE -= down * TRANSLATION_STEP_SIZE;
  }
  if (keystate[SDLK_a]) {
    CAMERA_CENTRE -= right * TRANSLATION_STEP_SIZE;
  }
  if (keystate[SDLK_d]) {
    CAMERA_CENTRE += right * TRANSLATION_STEP_SIZE;
  }
  if (keystate[SDLK_w]) {
    CAMERA_CENTRE += forward * TRANSLATION_STEP_SIZE;
  }
  if (keystate[SDLK_s]) {
    CAMERA_CENTRE -= forward * TRANSLATION_STEP_SIZE;
  }
}

void updateCameraRotation() {
  CAMERA_ROTATION_X[0] = vec3(1, 0, 0);
  CAMERA_ROTATION_X[1] = vec3(0, cos(PITCH), sin(PITCH));
  CAMERA_ROTATION_X[2] = vec3(0, -sin(PITCH), cos(PITCH));

  CAMERA_ROTATION_Y[0] = vec3(cos(YAW), 0, -sin(YAW));
  CAMERA_ROTATION_Y[1] = vec3(0, 1, 0);
  CAMERA_ROTATION_Y[2] = vec3(sin(YAW), 0, cos(YAW));

  CAMERA_ROTATION_Z[0] = vec3(cos(ROLL), sin(ROLL), 0);
  CAMERA_ROTATION_Z[1] = vec3(-sin(ROLL), cos(ROLL), 0);
  CAMERA_ROTATION_Z[2] = vec3(0, 0, 1);

  CAMERA_ROTATION = CAMERA_ROTATION_X * CAMERA_ROTATION_Y * CAMERA_ROTATION_Z;
}

void updateLightPosition(const Uint8 *keystate) {
  vec3 forward(0, 0, 1);
  vec3 right(1, 0, 0);
  vec3 down(0, 1, 0);

  if (keystate[SDLK_i]) {
    LIGHT_POSITION += forward * TRANSLATION_STEP_SIZE;
  }
  if (keystate[SDLK_k]) {
    LIGHT_POSITION -= forward * TRANSLATION_STEP_SIZE;
  }
  if (keystate[SDLK_j]) {
    LIGHT_POSITION -= right * TRANSLATION_STEP_SIZE;
  }
  if (keystate[SDLK_l]) {
    LIGHT_POSITION += right * TRANSLATION_STEP_SIZE;
  }
  if (keystate[SDLK_u]) {
    LIGHT_POSITION -= down * TRANSLATION_STEP_SIZE;
  }
  if (keystate[SDLK_o]) {
    LIGHT_POSITION += down * TRANSLATION_STEP_SIZE;
  }
}
