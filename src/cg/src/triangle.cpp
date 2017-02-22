#include "triangle.hpp"
#include "debug.hpp"
#include "glm/glm.hpp"
#include <ostream>

namespace cg {
  using namespace glm;
Triangle::Triangle(vec3 v0, vec3 v1, vec3 v2, vec3 color)
    : v0(v0), v1(v1), v2(v2), color(color) {
  ComputeNormal();
}

void Triangle::ComputeNormal() {
  normal = normalize(cross(e2(), e1()));
}

vec3 Triangle::e1() {
  return v1 - v0;
}

vec3 Triangle::e2() {
  return v2 -v0;
}


std::ostream& operator<<(std::ostream& os, Triangle& triangle) {
  os << "[" << triangle.v0 << ", " << triangle.v1 << ", " << triangle.v2 << "]";
}

}
