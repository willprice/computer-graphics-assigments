#ifndef LIBCG_TRIANGLE_H
#define LIBCG_TRIANGLE_H
// This code originates from TestModel.h provided by Carl Henrik Ek
#include "vertex.hpp"
#include <glm/glm.hpp>
#include <ostream>

namespace cg {
class Triangle {
public:
  Vertex v0;
  Vertex v1;
  Vertex v2;
  glm::vec3 e1;
  glm::vec3 e2;
  glm::vec3 normal;
  glm::vec3 color;
  float reflectance = 1;
  bool mirror = false;

  Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 color);
  Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 color, bool mirror);
  bool operator==(const Triangle &other);

  void ComputeNormal();

};
std::ostream &operator<<(std::ostream &os, Triangle &triangle);
};

#endif
