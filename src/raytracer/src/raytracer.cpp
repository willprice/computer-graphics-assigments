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
    *z += star_velocity*dt;
    if (*z <= 0) {
      *z = 1;
    }
  }

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

  world_to_camera[3][0] = -camera_centre.x;
  world_to_camera[3][1] = -camera_centre.y;
  world_to_camera[3][2] = -camera_centre.z;
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
    vec3 star_colour(1, 1, 1); // 0.2f * vec3(1, 1, 1) / (z * z);
    vec4 &world_star = stars[i];
    vec4 camera_star = world_to_camera*world_star;
    vec2 projection = project(screen, camera_star, focal_length);
    PutPixelSDL(screen, projection.x, projection.y, star_colour);
  }

  if (SDL_MUSTLOCK(screen))
    SDL_UnlockSurface(screen);

  SDL_UpdateRect(screen, 0, 0, 0, 0);
}


