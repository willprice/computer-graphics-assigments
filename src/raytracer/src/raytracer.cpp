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

static const float TRANSLATION_STEP_SIZE = 0.1;
static const float ROTATION_STEP_SIZE = 0.05;

vector<float> screen_pixel_centres_y(SCREEN_HEIGHT);
vector<float> screen_pixel_centres_x(SCREEN_WIDTH);
vector<Intersection> rayIntersections;

SDL_Surface* screen;
int t;
vector<Triangle> triangles;

float yaw = -.5;
float pitch = .5;
float roll = 0;
vec3 camera_centre(0, 0, -3);

mat3 camera_rotation;
mat3 camera_rotation_x;
mat3 camera_rotation_y;
mat3 camera_rotation_z;

vec3 light_position(0, -0.5, -0.7);
vec3 light_color = 14.f * vec3(1,1,1);


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

vec3 directLight(const Intersection &intersection);

int main(int argc, char* argv[] )
{
  screen = InitializeSDL( SCREEN_WIDTH, SCREEN_HEIGHT );
  t = SDL_GetTicks();	// Set start value for timer.

  LoadTestModel(triangles);
  calculateScreenPixelCentres();

  SDL_WM_GrabInput(SDL_GRAB_ON);

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
        vec3 illumination = directLight(closestIntersection);
        PutPixelSDL(screen, x, y, illumination);
      }
    }
  }
  SDL_UpdateRect(screen, 0, 0, 0, 0);
}

vec3 directLight(const Intersection &intersection) {
  vec3 intersection_to_light_source = - intersection.position + light_position;
  intersection_to_light_source = glm::normalize(intersection_to_light_source);
  vec3 normal = glm::normalize(triangles[intersection.triangleIndex].normal);

  float cosine_light_ray_to_surface_normal = glm::dot(intersection_to_light_source, normal);
  float distance_to_light_source = glm::length(intersection_to_light_source);
  float source_light_sphere_area = 4 * 3.142 * pow(distance_to_light_source, 2);
  float scalar = max(cosine_light_ray_to_surface_normal, 0.0f) / source_light_sphere_area;
  vec3 illumination = light_color * scalar;
  return illumination;
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
  int x;
  int y;
  SDL_GetRelativeMouseState(&x, &y);
  pitch += -y/(SCREEN_HEIGHT * 1.0);
  yaw += x/(SCREEN_WIDTH*1.0);
}

void updateCameraPosition(const Uint8 *keystate) {
  vec3 right(camera_rotation[0][0],
             camera_rotation[0][1],
             camera_rotation[0][2]
  );
  vec3 down(camera_rotation[1][0],
             camera_rotation[1][1],
             camera_rotation[1][2]
  );
  vec3 forward(camera_rotation[2][0],
            camera_rotation[2][1],
            camera_rotation[2][2]
  );
  if( keystate[SDLK_q] )
  {
    camera_centre += down*TRANSLATION_STEP_SIZE;
  }
  if( keystate[SDLK_e] )
  {
    camera_centre -= down*TRANSLATION_STEP_SIZE;
  }
  if( keystate[SDLK_a] )
  {
    camera_centre -= right*TRANSLATION_STEP_SIZE;
  }
  if( keystate[SDLK_d] )
  {
    camera_centre += right*TRANSLATION_STEP_SIZE;
  }
  if( keystate[SDLK_w] )
  {
    camera_centre += forward*TRANSLATION_STEP_SIZE;
  }
  if( keystate[SDLK_s] )
  {
    camera_centre -= forward*TRANSLATION_STEP_SIZE;
  }
}

void updateCameraRotation() {
  camera_rotation_x[0] = vec3(1, 0, 0);
  camera_rotation_x[1] = vec3(0, cos(pitch), sin(pitch));
  camera_rotation_x[2] = vec3(0, -sin(pitch), cos(pitch));

  camera_rotation_y[0] = vec3(cos(yaw), 0, -sin(yaw));
  camera_rotation_y[1] = vec3(0, 1, 0);
  camera_rotation_y[2] = vec3(sin(yaw), 0, cos(yaw));

  camera_rotation_z[0] = vec3(cos(roll), sin(roll), 0);
  camera_rotation_z[1] = vec3(-sin(roll), cos(roll), 0);
  camera_rotation_z[2] = vec3(0, 0, 1);

  camera_rotation = camera_rotation_x*camera_rotation_y*camera_rotation_z;
}
