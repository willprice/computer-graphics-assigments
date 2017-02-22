#include <ostream>
#include <glm/glm.hpp>
#include "debug.hpp"

namespace cg {
using glm::vec3;
using glm::vec2;
using glm::ivec2;
std::ostream &operator<<(std::ostream &os, vec3 &v) {
  os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
  return os;
}
std::ostream &operator<<(std::ostream &os, vec2 &v) {
  os << "(" << v.x << ", " << v.y << ")";
  return os;
}

std::ostream &operator<<(std::ostream &os, ivec2 &v) {
  os << "(" << v.x << ", " << v.y << ")";
  return os;
}
}
