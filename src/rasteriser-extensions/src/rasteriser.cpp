#include <iostream>
#include <map>
#include <set>

#include <cmath>
#include <csignal>

#include <SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/ext.hpp>

#include "SDLauxiliary.hpp"
#include "debug.hpp"
#include "interpolation.hpp"
#include "models/cornell_box.hpp"
#include "triangle.hpp"
#include "vertex_attributes.hpp"

using namespace std;
using namespace cg;
using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::ivec2;
using glm::mat3;
using glm::mat4;


// IMPORTANT: Abbreviations:
// CF = camera coordinate frame
// WF = world coordinate frame
// NDC = normalised device coordinates

/* ----------------------------------------------------------------------------*/
/* GLOBAL VARIABLES */

#define MOUSE_CONTROLS_ON 0

const int SCREEN_WIDTH = 500;
const int SCREEN_HEIGHT = 500;
const float WORLD_WIDTH = 2;
const float WORLD_HEIGHT = 2;
const float FOCAL_LENGTH = 2;

static const float TRANSLATION_STEP_SIZE = 0.01;

// z-coordinate of Far plane of viewing frustrum
const float f = -20;
// z-coordinate of Near plane of viewing frustrum
const float n = -FOCAL_LENGTH;
// x-coordinate of Top right of near plane of the viewing frustum
const float t = 1;
// y-coordinate of top Right of near plane of the viewing frustum
const float r = 1;
// y-coordinate of bottom Left of near plane of the viewing frustum
const float l = -1;
// x-coordinate of Bottom left of near plane of the viewing frustum
const float b = -1;
float _CF_TO_CLIP_SPACE_TRANSFORM_ARRAY[16] = {
        2 * n / (r - l), 0, 0, 0,
        0, 2 * n / (t - b), 0, 0,
        (r + l)/(r - l), (t + b)/(t - b), - (f + n)/(f - n), -1,
        0, 0, - (2 * f * n) / (f - n), 0
};
mat4 CF_TO_CLIP_SPACE_TRANSFORM = glm::make_mat4(_CF_TO_CLIP_SPACE_TRANSFORM_ARRAY);


float _VIEWPORT_TRANSFORM_ARRAY[16] = {
        0.5, 0, 0, 0.5,
        0, 0.5, 0, 0.5,
        0, 0, 0.5, 0.5,
        0, 0, 0, 1
};
mat4 VIEWPORT_TRANSFORM = glm::transpose(glm::make_mat4(_VIEWPORT_TRANSFORM_ARRAY));

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
float YAW = 0;
float PITCH = 0; // When using mouse set to M_PI_4 / 4
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

enum { AXIS_X, AXIS_Y, AXIS_Z };
/* ----------------------------------------------------------------------------*/
/* FUNCTIONS */

std::ostream & operator<<(std::ostream &o, vec4 &v) {
  o << "(" << v.x << ", "
           << v.y << ", "
           << v.z << ", "
           << v.w << ")";
  return o;
}

void Update();
void Draw();
void updateCameraRotation();
void updateCameraParameters(const Uint8 *keystate);
void updateCameraPosition(const Uint8 *keystate);
void updateCameraRotation(const Uint8 *keystate);
void updateLightPosition(const Uint8 *keystate);
float computeRenderTime();
void calculateScreenPixelCentres();
void vertexShader(const vec3 &vertexWF, VertexAttributes & pixel);
void constructPixelLine(Pixel start, Pixel end, vector<Pixel> &line);
void interpolate(Pixel a, Pixel b, vector<Pixel> &result);
void computeRows(const vector<Pixel> &vertexPixels, vector<Pixel> &leftPixels,
                 vector<Pixel> &rightPixels);
void drawRows(const vector<Pixel> &leftPixels,
              const vector<Pixel> &rightPixels);
void drawPolygon(vector<Vertex> &vertices, vector<VertexAttributes> attributes);
void pixelShader(const Pixel &pixel);
void computeVertexNormals(vector<Triangle> &triangles);
void updateVertexNormal(const set<Triangle *> &triangles, const Vertex &vertex,
                        vec3 normal);

vector<vec4> clip(vector<Vertex> &vertices);

vec3 worldToCamera(const vec3 &vertexWF);
vec4 worldToNDC(const vec3 &vertex);


vector<vec4> clipPolygonOnAxis(vector<vec4> &polygonVertices, unsigned int axis);

vector<vec4> clipPolygonsBehindCamera(vector<vec4> &polygonVertices);

vector<Pixel> viewportTransform(vector<vec4> verticesNDC, vector<VertexAttributes> attributes);

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
  SDL_SaveBMP(screen, "rasteriser-extensions-output.bmp");

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
    vector<VertexAttributes> attributes = {
            {vertices[0].position},
            {vertices[1].position},
            {vertices[2].position},
    };
    drawPolygon(vertices, attributes);
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
  // Interpolating pos3d linearly with the position divided by the z value.
  vec3 pos3d_step_size =
      (end.pos3d * end.zinv - start.pos3d * start.zinv) / float(max(N - 1, 1));
  for (size_t i = 0; i < result.size(); i++) {
    result[i].x = glm::round(start.x + x_step_size * i);
    result[i].y = glm::round(start.y + y_step_size * i);
    result[i].zinv = start.zinv + zinv_step_size * i;
    result[i].pos3d = (start.pos3d * start.zinv + pos3d_step_size * float(i)) /
                      result[i].zinv;
  }
}


vec4 perspectiveDivision(const vec4& vec) {
  vec4 vertexNDC;
  vertexNDC.x = vec.x / vec.w;
  vertexNDC.y = vec.y / vec.w;
  vertexNDC.z = vec.z / vec.w;
  vertexNDC.w = 1;
  return vertexNDC;
}

vector<vec4> perspectiveDivide(vector<vec4> & verticesClipSpace) {
  vector<vec4> verticesNDC;
  for (auto &vertexClipSpace : verticesClipSpace) {
    verticesNDC.push_back(perspectiveDivision(vertexClipSpace));
  }
  return verticesNDC;
}


void drawPolygon(vector<Vertex> &vertices, vector<VertexAttributes> attributes) {
  for (int i = 0; i < vertices.size(); ++i) {
    vertexShader(vertices[i].position, attributes[i]);
  }
  vector<vec4> verticesClipSpace = clip(vertices);
  vector<vec4> verticesNDC = perspectiveDivide(verticesClipSpace);
  vector<Pixel> vertexPixels = viewportTransform(verticesNDC, attributes);
  //viewportTransform
  vector<Pixel> leftPixels;
  vector<Pixel> rightPixels;
  computeRows(vertexPixels, leftPixels, rightPixels);
  drawRows(leftPixels, rightPixels);
}

vector<Pixel> viewportTransform(vector<vec4> verticesNDC, vector<VertexAttributes> attributes) {
  vector<Pixel> pixels(verticesNDC.size());
  for (size_t i = 0; i < verticesNDC.size(); ++i) {
    vec4 vertex = verticesNDC[i];
    pixels[i].pos3d = attributes[i].posWF;
    pixels[i].zinv = attributes[i].zinv;
    vec4 pixelVertex = VIEWPORT_TRANSFORM * vertex;
    pixels[i].x = int(SCREEN_WIDTH * pixelVertex.x);
    pixels[i].y = int(SCREEN_HEIGHT * pixelVertex.y);
  }

  return pixels;
}

vec4 homogenise(const vec3& vec) {
  vec4 homogenisedVec;
  homogenisedVec.x = vec.x;
  homogenisedVec.y = vec.y;
  homogenisedVec.z = vec.z;
  homogenisedVec.w = 1;
  return homogenisedVec;
}

std::vector<vec4> clip(std::vector<Vertex> &vertices) {
  assert(vertices.size() > 0);
  vector<vec4> verticesClipSpace;
  for (auto &vertex : vertices) {
    vec3 vertexCF = worldToCamera(vertex.position);
    vec4 vertexClipSpace = CF_TO_CLIP_SPACE_TRANSFORM * homogenise(vertexCF);
    verticesClipSpace.push_back(vertexClipSpace);
  }
  //verticesClipSpace = clipPolygonsBehindCamera(verticesClipSpace);
  //verticesClipSpace = clipPolygonOnAxis(verticesClipSpace, AXIS_X);
  //verticesClipSpace = clipPolygonOnAxis(verticesClipSpace, AXIS_Y);
  //verticesClipSpace = clipPolygonOnAxis(verticesClipSpace, AXIS_Z);
  return verticesClipSpace;
}

vector<vec4> clipPolygonsBehindCamera(vector<vec4> &polygonVertices) {
  vector<vec4> verticesInFrontOfCamera;

  vec4 & previousVertex = polygonVertices.back();
  // Normal for clipping against w = \epsilon is (0, 0, 0, 1), i.e. just the w component
  float previousDot = previousVertex.w;
  float currentDot;
  for (auto & vertex: polygonVertices) {
    currentDot = vertex.w;
    if (previousDot > 0 && currentDot < 0 ||
        previousDot < 0 && currentDot > 0) {
      float tForIntersection = previousVertex.w / (previousVertex.w - vertex.w);
      vec4 intersection = previousVertex + tForIntersection * (vertex - previousVertex);
      verticesInFrontOfCamera.push_back(intersection);
    }

    if (currentDot > 0) {
      verticesInFrontOfCamera.push_back(vertex);
    }
    previousVertex = vertex;
  }
  return verticesInFrontOfCamera;
}


// This clips according to Ken Joy's convex polygon clipping algorithm
// We iterate over the vertices of the polygon deciding whether each vertex is
// in or out, then if the previous vertex and the current vertex transition from
// out to in or in to out we compute the intersection of the line between the
// current and previous vertex and the plane and add that to our list of
// vertices making up the polygon.
vector<vec4> clipPolygonOnAxis(vector<vec4> &polygonVertices, unsigned int axis) {
  // The comments in this algorithm look at the case when axis = x, this
  // makes understanding the algorithm simpler, the comments hold for all
  // axes.

  vector<vec4> verticesInsidePlane;
  vec4 &previousVertex = polygonVertices.back();
  // e.g. for X axis, our normal is (1, 0, 0, 1) for w = -x or (-1, 0, 0, 1) for
  // w = x.

  // First we clip against w = x
  float previousDot = previousVertex[axis] + previousVertex.w;
  float currentDot;
  for (auto & vertex : polygonVertices) {
    currentDot = vertex[axis] + vertex.w;
    if (previousDot < 0 && currentDot > 0 ||
        previousDot > 0 && currentDot < 0) {
      // Line segment goes from outside to inside the plane, so we need to
      // calculate the intersection
      // our line from Q_1 to Q_2 can be written in parameteric form:
      // Q(t) = Q_1 + t (Q_2 - Q_1)
      // We need to find the t such that Q(t) lies on the plane, refer to the
      // notes above on how this works.

      float tForIntersection = (previousVertex.w - previousVertex[axis]) /
              ((previousVertex.w - previousVertex[axis]) - (vertex.w - vertex[axis]));
      vec4 intersection = previousVertex + tForIntersection * (vertex - previousVertex);
      verticesInsidePlane.push_back(intersection);
    }

    if (currentDot > 0) {
      verticesInsidePlane.push_back(vertex);
    }

    previousVertex = vertex;
  }
  return verticesInsidePlane;
}

vec3 worldToCamera(const vec3 &vertexWF) {
  return (vertexWF - CAMERA_CENTRE) * CAMERA_ROTATION;
}

vec4 worldToNDC(const vec3 &vertex) {
  vec3 vertexCF = worldToCamera(vertex);
  vec4 vertexCFHomogonised = homogenise(vertexCF);
  vec4 vertexCS = CF_TO_CLIP_SPACE_TRANSFORM * vertexCFHomogonised;
  vec4 vertexNDC = perspectiveDivision(vertexCS);
  return vertexNDC;
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
  int x = pixel.x;
  int y = pixel.y;
  if (pixel.zinv > depthBuffer[y][x]) {
    // Vector from surface point to the light source

    vec3 surface_to_light = vec3(worldToNDC(LIGHT_POSITION))  - pixel.pos3d;

    // Compute illumination of vertex
    #pragma clang diagnostic push
    #pragma ide diagnostic ignored "IncompatibleTypes"
    float surface_to_light_distance = length(surface_to_light);
    #pragma clang diagnostic pop
    float scale = (4 * M_PI * surface_to_light_distance * surface_to_light_distance);
    vec3 direct_illumination =
            (LIGHT_POWER *
             max(dot(normalize(surface_to_light), normalize(currentNormal)), 0.0f)) /
            scale;

    vec3 illumination = currentReflectance *
                        (direct_illumination + INDIRECT_LIGHT_POWER_PER_AREA);
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

void constructPixelLine(Pixel start, Pixel end, vector<Pixel> &line) {
  ivec2 delta = glm::abs(ivec2(start.x, start.y) - ivec2(end.x, end.y));

  uint pixel_count = (uint)glm::max(delta.x, delta.y) + 1;

  line.resize(pixel_count);
  interpolate(start, end, line);
}

void vertexShader(const vec3 &vertexWF, VertexAttributes & attributes) {
  vec3 vertexCF = worldToCamera(vertexWF);
  attributes.posWF = vertexWF;

  attributes.zinv = 1 / vertexCF.z;
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

