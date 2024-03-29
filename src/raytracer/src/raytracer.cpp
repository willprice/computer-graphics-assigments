#include "omp.h"
#include <SDL.h>
#include <cmath>
#include <csignal>
#include <glm/glm.hpp>
#include <iostream>

#include "SDLauxiliary.hpp"
#include "interpolation.hpp"
#include "intersection.hpp"
#include "models/cornell_box.hpp"
#include "triangle.hpp"

using namespace std;
using namespace cg;
using glm::vec2;
using glm::vec3;
using glm::mat3;

/* ----------------------------------------------------------------------------*/
/* GLOBAL VARIABLES */

#define MOUSE_CONTROLS_ON 0

const int SCREEN_WIDTH = 500;
const int SCREEN_HEIGHT = 500;
const float WORLD_WIDTH = 2;
const float WORLD_HEIGHT = 2;
const float FOCAL_LENGTH = 2;

static const float TRANSLATION_STEP_SIZE = 0.1;

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
vec3 CAMERA_CENTRE(0, 0, -3);

mat3 CAMERA_ROTATION;
mat3 CAMERA_ROTATION_X;
mat3 CAMERA_ROTATION_Y;
mat3 CAMERA_ROTATION_Z;

vec3 LIGHT_POSITION(0, -0.5, -0.7);
vec3 LIGHT_COLOR = 14.f * vec3(1, 1, 1);

vec3 INDIRECT_LIGHT = 0.5f * vec3(1, 1, 1);

/* ----------------------------------------------------------------------------*/
/* FUNCTIONS */

void Update();
void Draw();
void initialiseCameraRotationMatrix();
void updateCameraParameters(const Uint8 *keystate);
void updateCameraPosition(const Uint8 *keystate);
void updateCameraRotation(const Uint8 *keystate);
void updateLightPosition(const Uint8 *keystate);
bool closest_intersection(vec3 start, vec3 direction, Intersection &closestIntersection);
float computeRenderTime();
void calculateScreenPixelCentres();
vec3 directLight(const Intersection &intersection);

int main(int argc, char *argv[]) {
  screen = InitializeSDL(SCREEN_WIDTH, SCREEN_HEIGHT);
  TIME = SDL_GetTicks(); // Set start value for timer.
  signal(SIGINT, SIG_DFL);

  LoadTestModel(triangles);
  calculateScreenPixelCentres();

  if (MOUSE_CONTROLS_ON) {
    SDL_WM_GrabInput(SDL_GRAB_ON);
    SDL_WarpMouse(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
  }

  while (NoQuitMessageSDL()) {
    Update();
    Draw();
  }
  SDL_SaveBMP(screen, "raytracer-output.bmp");

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

  initialiseCameraRotationMatrix();

#pragma omp parallel
  {
#pragma omp for
    for (int y = 0; y < screen_pixel_centres_y.size(); y++) {
      for (int x = 0; x < screen_pixel_centres_x.size(); x++) {
        vec3 pixel_centre(screen_pixel_centres_x[x], screen_pixel_centres_y[y],
                          FOCAL_LENGTH);
        Intersection closestIntersection;
        if (closest_intersection(CAMERA_CENTRE, CAMERA_ROTATION * pixel_centre, closestIntersection)) {
          vec3 reflected_light(0, 0, 0);
          vec3 light_to_previous =
              -LIGHT_POSITION + closestIntersection.position;
          Intersection potential_occlusion;
          closest_intersection(LIGHT_POSITION, light_to_previous, potential_occlusion);
          Triangle triangle = triangles[closestIntersection.triangleIndex];
          if (potential_occlusion.triangleIndex ==
              closestIntersection.triangleIndex) {
            vec3 illumination =
                directLight(closestIntersection) + INDIRECT_LIGHT;
            reflected_light = triangle.color * illumination;
          } else {
            reflected_light = triangle.color * INDIRECT_LIGHT;
          }
          PutPixelSDL(screen, x, y, reflected_light);
        }
      }
    }
  }
  SDL_UpdateRect(screen, 0, 0, 0, 0);
}

vec3 directLight(const Intersection &intersection) {
  vec3 intersection_to_light_source = -intersection.position + LIGHT_POSITION;
  float distance_to_light_source = glm::length(intersection_to_light_source);
  intersection_to_light_source = glm::normalize(intersection_to_light_source);
  vec3 normal = glm::normalize(triangles[intersection.triangleIndex].normal);

  float cosine_light_ray_to_surface_normal =
      glm::dot(intersection_to_light_source, normal);
  float source_light_sphere_area = 4 * 3.142 * pow(distance_to_light_source, 2);
  float scalar =
      max(cosine_light_ray_to_surface_normal, 0.0f) / source_light_sphere_area;
  vec3 illumination = LIGHT_COLOR * scalar;
  return illumination;
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

bool closest_intersection(vec3 start, vec3 direction, Intersection &closestIntersection) {
  vector<Intersection> validIntersection;
  for (int i = 0; i < triangles.size(); i++) {
    Triangle triangle = triangles[i];
    mat3 A(-direction, triangle.e1, triangle.e2);
    vec3 b = start - triangle.v0.position;
    if (glm::determinant(A) == 0) {
      continue;
    }
    vec3 intersection_point = glm::inverse(A) * b;
    float u = intersection_point.y;
    float v = intersection_point.z;
    float t = intersection_point.x;
    if (u >= 0 && v >= 0 && u + v <= 1 && t >= 0) {
      Intersection intersection;
      intersection.distance = glm::length(t * direction);
      intersection.position = start + t * direction;
      intersection.triangleIndex = i;
      validIntersection.push_back(intersection);
    }
  }

  if (validIntersection.size() == 0) {
    return false;
  }

  closestIntersection = validIntersection.front();
  for (Intersection intersection : validIntersection) {
    if (intersection.distance < closestIntersection.distance) {
      closestIntersection = intersection;
    }
  }
  return true;
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

void initialiseCameraRotationMatrix() {
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
