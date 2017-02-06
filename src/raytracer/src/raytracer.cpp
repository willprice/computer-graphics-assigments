#include <SDL.h>
#include <cmath>
#include <glm/glm.hpp>
#include <iostream>
#include <map>

#include "interpolation.hpp"
#include "projection.hpp"
#include "SDLauxiliary.hpp"
#include "models/cornell_box.hpp"
#include "triangle.hpp"
#include "intersection.hpp"

using namespace std;
using namespace cg;
using glm::vec2;
using glm::vec3;
using glm::mat3;

/* ----------------------------------------------------------------------------*/
/* GLOBAL VARIABLES                                                            */

const int SCREEN_WIDTH = 500;
const int SCREEN_HEIGHT = 500;
const float WORLD_WIDTH = 2;
const float WORLD_HEIGHT = 2;
const float WORLD_DEPTH = 2;
const float FOCAL_LENGTH = 2;

static const double TRANSLATION_STEP_SIZE = 0.1;
static const double ROTATION_STEP_SIZE = 0.05;

vector<float> screen_pixel_centres_y(SCREEN_HEIGHT);
vector<float> screen_pixel_centres_x(SCREEN_WIDTH);
vector<Intersection> rayIntersections;

SDL_Surface* screen;
int t;
vector<Triangle> triangles;

vec3 camera_centre(0, 0, -3);
mat3 camera_rotation;
float yaw = 0;



/* ----------------------------------------------------------------------------*/
/* FUNCTIONS                                                                   */

float randomProbability() {
  return float(rand()) / float(RAND_MAX);
}


void Update();
void Draw();

void updateCameraRotation();

void updateCameraParameters();

void updateCameraPosition(const Uint8 *keystate);

void updateCameraRotation(const Uint8 *keystate);
bool closest_intersection(vec3 start, vec3 direction, const vector<Triangle>& triangles, Intersection& closestIntersection);

float computeRenderTime();

void calculateScreenPixelCentres();

int main(int argc, char* argv[] )
{
  screen = InitializeSDL( SCREEN_WIDTH, SCREEN_HEIGHT );
  t = SDL_GetTicks();	// Set start value for timer.

  LoadTestModel(triangles);
  calculateScreenPixelCentres();

  while( NoQuitMessageSDL() )
  {
    Update();
    Draw();
  }

  return 0;
}

void Update()
{
  cout << "Render time: " << computeRenderTime() << " ms." << endl;
  updateCameraParameters();
}


void Draw()
{
  SDL_FillRect(screen, 0, 0);

  if (SDL_MUSTLOCK(screen))
    SDL_LockSurface(screen);

  if (SDL_MUSTLOCK(screen))
    SDL_UnlockSurface(screen);


  updateCameraRotation();

  for (int y = 0; y < screen_pixel_centres_y.size(); y++) {
    for (int x = 0; x < screen_pixel_centres_x.size(); x++) {
      vec3 pixel_centre(screen_pixel_centres_x[x], screen_pixel_centres_y[y]
              , FOCAL_LENGTH);
      Intersection closestIntersection;
      if (closest_intersection(camera_centre, camera_rotation*pixel_centre, triangles, closestIntersection)) {
        Triangle triangle = triangles[closestIntersection.triangleIndex];
        PutPixelSDL(screen, x, y, triangle.color);
      }
    }
  }
  SDL_UpdateRect(screen, 0, 0, 0, 0);
}


void calculateScreenPixelCentres() {
  vec3 topLeft = vec3((-WORLD_WIDTH / 2) + (WORLD_WIDTH / (2 * SCREEN_WIDTH)),
                      (-WORLD_HEIGHT / 2) + (WORLD_HEIGHT / (2 * SCREEN_HEIGHT)),
                      FOCAL_LENGTH);

  vec3 bottomRight = vec3((WORLD_WIDTH / 2) - (WORLD_WIDTH / (2 * SCREEN_WIDTH)),
                          (WORLD_HEIGHT / 2) - (WORLD_HEIGHT / (2 * SCREEN_HEIGHT)),
                          FOCAL_LENGTH);

  vec3 bottomLeft = vec3(topLeft.x, bottomRight.y, FOCAL_LENGTH);
  vec3 topRight = vec3(bottomRight.x, topLeft.y, FOCAL_LENGTH);
  interpolate(topLeft.y, bottomLeft.y, screen_pixel_centres_y);
  interpolate(topLeft.x, topRight.x, screen_pixel_centres_x);
}

bool closest_intersection(vec3 start, vec3 direction, const vector<Triangle>& triangles, Intersection& closestIntersection) {
  vector<Intersection> validIntersection;
  for (int i = 0; i < triangles.size(); i++) {
    Triangle triangle = triangles[i];
    mat3 A(-direction, triangle.e1(), triangle.e2());
    vec3 b = start - triangle.v0;
    if (glm::determinant(A) == 0) {
      continue;
    }
    vec3 intersection_point = glm::inverse(A)*b;
    float u = intersection_point.y;
    float v = intersection_point.z;
    float t = intersection_point.x;
    if (u >= 0 && v >= 0 && u + v <= 1 && t >= 0) {
      Intersection intersection;
      intersection.distance = glm::length(t*direction);
      intersection.position = start + t*direction;
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
  float dt = float(t2-t);
  t = t2;
  return dt;
}

void updateCameraParameters() {
  Uint8* keystate = SDL_GetKeyState(0 );
  updateCameraPosition(keystate);
  updateCameraRotation(keystate);
}

void updateCameraRotation(const Uint8 *keystate) {
  if( keystate[SDLK_RIGHT] )
  {
    yaw += ROTATION_STEP_SIZE;
  }
  if( keystate[SDLK_LEFT] )
  {
    yaw -= ROTATION_STEP_SIZE;
  }
}

void updateCameraPosition(const Uint8 *keystate) {
  if( keystate[SDLK_w] )
  {
    camera_centre.y -= TRANSLATION_STEP_SIZE;
  }
  if( keystate[SDLK_s] )
  {
    camera_centre.y += TRANSLATION_STEP_SIZE;
  }
  if( keystate[SDLK_a] )
  {
    camera_centre.x -= TRANSLATION_STEP_SIZE;
  }
  if( keystate[SDLK_d] )
  {
    camera_centre.x += TRANSLATION_STEP_SIZE;
  }
  if( keystate[SDLK_e] )
  {
    camera_centre.z += TRANSLATION_STEP_SIZE;
  }
  if( keystate[SDLK_q] )
  {
    camera_centre.z -= TRANSLATION_STEP_SIZE;
  }
}

void updateCameraRotation() {
  camera_rotation[0] = vec3(cos(yaw), 0, -sin(yaw));
  camera_rotation[1] = vec3(0, 1, 0);
  camera_rotation[2] = vec3(sin(yaw), 0, cos(yaw));
}
