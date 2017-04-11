#include <glm/glm.hpp>
#include "SDLauxiliary.hpp"

using glm::vec3;

namespace cg {
SDL_Surface *InitializeSDL(int width, int height, bool fullscreen) {
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
    std::cout << "Could not init SDL: " << SDL_GetError() << std::endl;
    exit(1);
  }
  atexit(SDL_Quit);

  Uint32 flags = SDL_SWSURFACE;
  if (fullscreen)
    flags |= SDL_FULLSCREEN;

  SDL_Surface *surface = 0;
  surface = SDL_SetVideoMode(width, height, 32, flags);
  if (surface == 0) {
    std::cout << "Could not set video mode: " << SDL_GetError() << std::endl;
    exit(1);
  }
  return surface;
}

bool NoQuitMessageSDL() {
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    if (e.type == SDL_QUIT)
      return false;
    if (e.type == SDL_KEYDOWN)
      if (e.key.keysym.sym == SDLK_ESCAPE)
        return false;
  }
  return true;
}

// TODO: Does this work on all platforms?
void PutPixelSDL(SDL_Surface *surface, int x, int y, glm::vec3 color) {
  if (x < 0 || surface->w <= x || y < 0 || surface->h <= y)
    return;

  Uint8 r = Uint8(glm::clamp(255 * color.r, 0.f, 255.f));
  Uint8 g = Uint8(glm::clamp(255 * color.g, 0.f, 255.f));
  Uint8 b = Uint8(glm::clamp(255 * color.b, 0.f, 255.f));

  Uint32 *p = (Uint32 *)surface->pixels + y * surface->pitch / 4 + x;
  *p = SDL_MapRGB(surface->format, r, g, b);
}

vec3 GetPixelSDL(SDL_Surface *surface, int x, int y)
{
  int bytes_per_pixel = surface->format->BytesPerPixel;
  /* Here p is the address to the pixel we want to retrieve */
  Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bytes_per_pixel;

  Uint32 sdl_pixel_value;
  vec3 pixel_value;
  switch(bytes_per_pixel) {
    case 1:
      sdl_pixel_value = *p;
      break;

    case 2:
      sdl_pixel_value = *(Uint16 *)p;
      break;

    case 3:
      if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
        sdl_pixel_value = p[0] << 16 | p[1] << 8 | p[2];
      else
        sdl_pixel_value = p[0] | p[1] << 8 | p[2] << 16;

    case 4:
      sdl_pixel_value = *(Uint32 *)p;
      break;
    default:
      sdl_pixel_value = 0;       /* shouldn't happen, but avoids warnings  - Nice one SDL guys!*/
  }
  uint8_t r, g, b;
  SDL_GetRGB(sdl_pixel_value, surface->format, &r, &g, &b);
  pixel_value.r = r/255.f;
  pixel_value.g = g/255.f;
  pixel_value.b = b/255.f;
  return pixel_value;
}
};
