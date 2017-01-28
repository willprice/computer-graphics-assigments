#ifndef SDL_AUXILIARY_H
#define SDL_AUXILIARY_H

#include "SDL.h"
#include <glm/glm.hpp>
#include <iostream>


namespace cg {
  // Initializes SDL (video and timer). SDL creates a window where you can draw.
  // A pointer to this SDL_Surface is returned. After calling this function
  // you can use the function PutPixelSDL to do the actual drawing.
  SDL_Surface *InitializeSDL(int width, int height, bool fullscreen = false);

  // Checks all events/messages sent to the SDL program and returns true as long
  // as no quit event has been received.
  bool NoQuitMessageSDL();

  // Draw a pixel on a SDL_Surface. The color is represented by a glm:vec3 which
  // specifies the red, green and blue component with numbers between zero and
  // one. Before calling this function you should call:
  // if( SDL_MUSTLOCK( surface ) )
  //     SDL_LockSurface( surface );
  // After calling this function you should call:
  // if( SDL_MUSTLOCK( surface ) )
  //     SDL_UnlockSurface( surface );
  // SDL_UpdateRect( surface, 0, 0, 0, 0 );
  void PutPixelSDL(SDL_Surface *surface, int x, int y, glm::vec3 color);
}

#endif
