#ifndef LIBCG_PROJECTION_H
#define LIBCG_PROJECTION_H

#include "glm/common.hpp"
#include <SDL_video.h>

namespace cg {
  glm::vec2 project(SDL_Surface *surface, glm::vec4 point, float focal_length);
};

#endif
