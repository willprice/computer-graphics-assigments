#ifndef VERTEX_HPP
#define VERTEX_HPP

class Vertex {
public:
  glm::vec3 position;
  mutable glm::vec3 normal;

  Vertex() {}
  Vertex(glm::vec3 position) : position(position) {}

  bool operator==(const Vertex& other) {
    return position == other.position;
  }
};

#endif //VERTEX_HPP
