#include "glm/common.hpp"
#include <SDL_video.h>
#include <glm/vec3.hpp>

using namespace glm;

namespace cg {
  glm::vec2 project(SDL_Surface *surface, glm::vec3 point, float focal_length) {
    vec2 projection;
    projection.x = focal_length * point.x / point.z + surface->w / 2;
    projection.y = focal_length * point.y / point.z + surface->h / 2;
    return projection;
  }
}
