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

vector<float> screen_pixel_centres_y(SCREEN_HEIGHT);
vector<float> screen_pixel_centres_x(SCREEN_WIDTH);
vector<Intersection> rayIntersections;

SDL_Surface* screen;
int t;
vector<Triangle> triangles;

vec3 camera_centre(0, 0, -3);


/* ----------------------------------------------------------------------------*/
/* FUNCTIONS                                                                   */

float randomProbability() {
  return float(rand()) / float(RAND_MAX);
}


void Update();
void Draw();

int main( int argc, char* argv[] )
{
  screen = InitializeSDL( SCREEN_WIDTH, SCREEN_HEIGHT );
  t = SDL_GetTicks();	// Set start value for timer.

  LoadTestModel(triangles);

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

  while( NoQuitMessageSDL() )
  {
    Update();
    Draw();
  }

  return 0;
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
void Update()
{
  // Compute frame time:
  int t2 = SDL_GetTicks();
  float dt = float(t2-t);
  t = t2;
  cout << "Render time: " << dt << " ms." << endl;



  Uint8* keystate = SDL_GetKeyState( 0 );
  if( keystate[SDLK_UP] )
  {
    camera_centre.y -= 0.01;
  }
  if( keystate[SDLK_DOWN] )
  {
    camera_centre.y += 0.01;
  }
  if( keystate[SDLK_LEFT] )
  {
    camera_centre.x -= 0.01;
  }
  if( keystate[SDLK_RIGHT] )
  {
    camera_centre.x += 0.01;
  }
  if( keystate[SDLK_w] )
  {
    camera_centre.z += 0.0001;
  }
  if( keystate[SDLK_s] )
  {
    camera_centre.z -= 0.0001;
  }
}

void Draw()
{
  SDL_FillRect(screen, 0, 0);

  if (SDL_MUSTLOCK(screen))
    SDL_LockSurface(screen);

  if (SDL_MUSTLOCK(screen))
    SDL_UnlockSurface(screen);


  for (int y = 0; y < screen_pixel_centres_y.size(); y++) {
    for (int x = 0; x < screen_pixel_centres_x.size(); x++) {
      vec3 pixel_centre(screen_pixel_centres_x[x], screen_pixel_centres_y[y]
              , FOCAL_LENGTH);
      Intersection closestIntersection;
      if (closest_intersection(camera_centre, pixel_centre, triangles, closestIntersection)) {
        Triangle triangle = triangles[closestIntersection.triangleIndex];
        PutPixelSDL(screen, x, y, triangle.color);
      }
    }
  }
  SDL_UpdateRect(screen, 0, 0, 0, 0);
}
