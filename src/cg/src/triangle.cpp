#include "triangle.hpp"
#include "debug.hpp"
#include "glm/glm.hpp"
#include "vertex.hpp"
#include <ostream>
#include <iostream>

namespace cg {
using namespace glm;
Triangle::Triangle(vec3 _v0, vec3 _v1, vec3 _v2, vec3 color)
    : v0(_v0), v1(_v1), v2(_v2), color(color) {
  ComputeNormal();
}

Triangle::Triangle(vec3 _v0, vec3 _v1, vec3 _v2, vec3 color, bool mirror)
    : v0(_v0), v1(_v1), v2(_v2), color(color), mirror(mirror) {
  ComputeNormal();
}

Triangle::Triangle(vec3 _v0, vec3 _v1, vec3 _v2, vec3 color, bool mirror, bool texturesEnabled, vec2 t0, vec2 t1, vec2 t2)
    : v0(_v0, t0), v1(_v1, t1), v2(_v2, t2), color(color), mirror(mirror), texturesEnabled(texturesEnabled) {
  ComputeNormal();
  //ComputeTexturePositions();
}

void Triangle::ComputeNormal() {
  e1 = v1.position - v0.position;
  e2 = v2.position - v0.position;
  normal = normalize(cross(e2, e1));
}
/*
void Triangle::ComputeTexturePositions(glm::vec2 texturePosition, int vertexNum) {
  if (vertexNum  == 0) {
    v0.texturePosition = texturePosition;
  } else if (vertexNum == 1) {
    v1.texturePosition = texturePosition;
  } else if (vertexNum == 2) {
    v2.texturePosition = texturePosition;
  } else {
    std::cout << "Invalid Vertex Number" << std::endl;
  }
}
*/

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
