#include "omp.h"
#include <SDL.h>
#include <cmath>
#include <glm/glm.hpp>
#include <iostream>

#include "SDLauxiliary.hpp"
#include "interpolation.hpp"
#include "models/cornell_box.hpp"
#include "triangle.hpp"

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

static const float TRANSLATION_STEP_SIZE = 0.001;
static const float ROTATION_STEP_SIZE = 0.005;

vector<float> screen_pixel_centres_y(SCREEN_HEIGHT);
vector<float> screen_pixel_centres_x(SCREEN_WIDTH);

SDL_Surface *screen;
int TIME;
vector<Triangle> triangles;

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
vec3 LIGHT_COLOR = 14.f * vec3(1, 1, 1);

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

void vertexShader(const vec3 &v, ivec2 &p);

vector<ivec2> constructPixelLine(ivec2 start, ivec2 end);

ivec2 project(vec3 point);

int main(int argc, char *argv[]) {
  screen = InitializeSDL(SCREEN_WIDTH, SCREEN_HEIGHT);
  TIME = SDL_GetTicks(); // Set start value for timer.

  LoadTestModel(triangles);
  calculateScreenPixelCentres();

  while (NoQuitMessageSDL()) {
    Update();
    Draw();
  }

  return 0;
}

void Update() {
  cout << "Render time: " << computeRenderTime() << " ms." << endl;
  Uint8 *keystate = SDL_GetKeyState(0);
  updateCameraParameters(keystate);
  updateLightPosition(keystate);
}

void Draw() {
  SDL_FillRect(screen, 0, 0);

  if (SDL_MUSTLOCK(screen))
    SDL_LockSurface(screen);

  if (SDL_MUSTLOCK(screen))
    SDL_UnlockSurface(screen);

  updateCameraRotation();

  for (int i = 0; i < triangles.size(); ++i) {
    vector<ivec2> vertices(3);
    vertices[0] = project(triangles[i].v0);
    vertices[1] = project(triangles[i].v1);
    vertices[2] = project(triangles[i].v2);

    vector<ivec2> edge1 = constructPixelLine(vertices[0], vertices[1]);
    vector<ivec2> edge2 = constructPixelLine(vertices[1], vertices[2]);
    vector<ivec2> edge3 = constructPixelLine(vertices[0], vertices[2]);

    vector<vector<ivec2>> edges = {edge1, edge2, edge3};

    for (auto edge : edges) {
      for (auto pixel : edge) {
        //ivec2 projPos;
        //vertexShader(pixel, projPos);
        vec3 color(1, 1, 1);
        PutPixelSDL(screen, pixel.x, pixel.y, color);
      }
    }
  }

  SDL_UpdateRect(screen, 0, 0, 0, 0);
}

ivec2 project(vec3 point) {
  ivec2 projection;
   vertexShader(point, projection);
   return projection;
}

vector<ivec2> constructPixelLine(ivec2 start, ivec2 end) {
  ivec2 delta = glm::abs(start - end);
  int pixel_count = glm::max( delta.x, delta.y ) + 1;

  vector<ivec2> line(pixel_count);

  interpolate(start, end, line);
  return line;
}

void vertexShader(const vec3 &world_point, ivec2 &image_point) {
  vec3 point = (world_point - CAMERA_CENTRE) * CAMERA_ROTATION;
  image_point.x = (SCREEN_WIDTH / WORLD_WIDTH) * FOCAL_LENGTH * point.x / point.z + SCREEN_WIDTH/2;
  image_point.y = (SCREEN_HEIGHT / WORLD_HEIGHT) * FOCAL_LENGTH * point.y / point.z + SCREEN_HEIGHT / 2;
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
