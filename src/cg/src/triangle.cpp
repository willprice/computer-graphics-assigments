#include "triangle.hpp"
#include "debug.hpp"
#include "glm/glm.hpp"
#include "vertex.hpp"
#include <ostream>

namespace cg {
using namespace glm;
Triangle::Triangle(vec3 _v0, vec3 _v1, vec3 _v2, vec3 color)
    : v0(_v0), v1(_v1), v2(_v2), color(color) {
  ComputeNormal();
}

void Triangle::ComputeNormal() {
  e1 = v1.position - v0.position;
  e2 = v2.position - v0.position;
  normal = normalize(cross(e2, e1));
}


bool Triangle::operator==(const Triangle &other) {
  // NOTE: maybe check normal as well?

  return v0 == other.v0 && v1 == other.v1 && v2 == other.v2 &&
         color == other.color;
}

std::ostream &operator<<(std::ostream &os, Triangle &triangle) {
  os << "[" << triangle.v0.position << ", " << triangle.v1.position << ", "
     << triangle.v2.position << "]";
  return os;
}
}
