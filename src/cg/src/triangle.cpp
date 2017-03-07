#include "triangle.hpp"
#include "debug.hpp"
#include "glm/glm.hpp"
#include "vertex.hpp"
#include <ostream>

namespace cg {
using namespace glm;
Triangle::Triangle(vec3 v0, vec3 v1, vec3 v2, vec3 color)
    : v0(v0), v1(v1), v2(v2), color(color) {
  ComputeNormal();
}

void Triangle::ComputeNormal() { normal = normalize(cross(e2(), e1())); }

vec3 Triangle::e1() { return v1.position - v0.position; }

vec3 Triangle::e2() { return v2.position - v0.position; }

bool Triangle::operator==(const Triangle &other) {
  // NOTE: maybe check normal as well?

  return v0 == other.v0 && v1 == other.v1 && v2 == other.v2 &&
         color == other.color;
}

std::ostream &operator<<(std::ostream &os, Triangle &triangle) {
  os << "[" << triangle.v0.position << ", " << triangle.v1.position << ", "
     << triangle.v2.position << "]";
}
}
