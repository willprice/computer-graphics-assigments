#include "triangle.hpp"


namespace cg {
Triangle::Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 color)
    : v0(v0), v1(v1), v2(v2), color(color) {
  ComputeNormal();
}

void Triangle::ComputeNormal() {
  glm::vec3 e1 = v1 - v0;
  glm::vec3 e2 = v2 - v0;
  normal = glm::normalize(glm::cross(e2, e1));
}
}
