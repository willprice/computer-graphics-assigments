#include "debug.hpp"
#include <glm/glm.hpp>
#include <ostream>

namespace cg {
using glm::vec3;
using glm::vec2;
using glm::ivec2;
std::ostream &operator<<(std::ostream &os, const vec3 &v) {
  os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
  return os;
}
std::ostream &operator<<(std::ostream &os, const vec2 &v) {
  os << "(" << v.x << ", " << v.y << ")";
  return os;
}

std::ostream &operator<<(std::ostream &os, const ivec2 &v) {
  os << "(" << v.x << ", " << v.y << ")";
  return os;
}
}
