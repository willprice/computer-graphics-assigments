#ifndef LIBCG_INTERSECTION_HPP
#define LIBCG_INTERSECTION_HPP
#include "glm/glm.hpp"

struct Intersection {
  glm::vec3 position;
  float distance;
  int triangleIndex;
};

#endif
