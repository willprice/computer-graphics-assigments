#ifndef VERTEX_HPP
#define VERTEX_HPP
#include <glm/glm.hpp>

class Vertex {
public:
  glm::vec3 position;
  glm::vec3 normal;
  float reflectance = 1;

  Vertex() {}
  Vertex(glm::vec3 position) : position(position) {}

  bool operator==(const Vertex& other) {
    return position == other.position;
  }
};

#endif //VERTEX_HPP
