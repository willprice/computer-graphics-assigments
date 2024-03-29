#ifndef VERTEX_HPP
#define VERTEX_HPP
#include <glm/glm.hpp>

class Vertex {
public:
  glm::vec3 position;
  glm::vec3 normal; // NOTE: Not currently used but can be for an extension
  glm::vec2 texturePosition;
  int vertexNum;

  Vertex() {}
  Vertex(glm::vec3 position) : position(position) {}
  Vertex(glm::vec3 position, glm::vec2 texturePosition) : position(position), texturePosition(texturePosition) {}

  bool operator==(const Vertex &other) { return position == other.position; }
};

#endif // VERTEX_HPP
