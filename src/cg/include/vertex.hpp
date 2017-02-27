#ifndef VERTEX_HPP
#define VERTEX_HPP

class Vertex {
public:
  Vertex() {}
  Vertex(glm::vec3 position) : position(position) {}
  glm::vec3 position;
};

#endif //VERTEX_HPP
