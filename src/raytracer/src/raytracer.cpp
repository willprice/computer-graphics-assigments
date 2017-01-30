#include <SDL.h>
#include <cmath>
#include <glm/glm.hpp>
#include <iostream>

#include "interpolation.hpp"
#include "projection.hpp"
#include "SDLauxiliary.hpp"
#include "models/cornell_box.hpp"

using namespace std;
using namespace cg;
using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat3;
using glm::mat4;

/* ----------------------------------------------------------------------------*/
/* GLOBAL VARIABLES                                                            */

const int SCREEN_WIDTH = 500;
const int SCREEN_HEIGHT = 500;
const float star_velocity = -0.0001;
SDL_Surface* screen;
int t;
vector<vec4> stars(1000);

vec4 camera_centre(0, 0, 0, 1);
mat4 world_to_camera = {
        vec4(1, 0, 0, 0),
        vec4(0, 1, 0, 0),
        vec4(0, 0, 1, 0),
        vec4(-camera_centre.x, -camera_centre.y, -camera_centre.z, 1)
};
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

  for (size_t i = 0; i < stars.size(); i++) {
    stars[i].x = rescale(-1, 1, randomProbability());
    stars[i].y = rescale(-1, 1, randomProbability());
    stars[i].z = randomProbability();
    stars[i][3] = 1;
  }

  while( NoQuitMessageSDL() )
  {
    Update();
    Draw();
  }

  return 0;
}

void Update()
{
  // Compute frame time:
  int t2 = SDL_GetTicks();
  float dt = float(t2-t);
  t = t2;
  cout << "Render time: " << dt << " ms." << endl;

  for (size_t i = 0; i < stars.size(); i++) {
    float *z = &stars[i].z;
    if (*z > 0) {
      *z += star_velocity*dt;
    } else {
      *z = 1;
    }
  }
}

void Draw()
{
  SDL_FillRect(screen, 0, 0);

  if (SDL_MUSTLOCK(screen))
    SDL_LockSurface(screen);


  float focal_length = 10;
  for (size_t i = 0; i < stars.size(); i++) {
    float z = stars[i].z;
    // Star colour is proportional to the inverse square of distance (standard photon equation)
    vec3 star_colour = 0.2f * vec3(1, 1, 1) / (z * z);
    vec4 &world_star = stars[i];
    vec4 camera_star = world_to_camera*world_star;
    vec2 projection = project(screen, camera_star, focal_length);
    PutPixelSDL(screen, projection.x, projection.y, star_colour);
  }

  if (SDL_MUSTLOCK(screen))
    SDL_UnlockSurface(screen);

  SDL_UpdateRect(screen, 0, 0, 0, 0);
}


