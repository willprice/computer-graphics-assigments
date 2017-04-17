#ifndef COMPUTER_GRAPHICS_ASSIGNMENTS_VERTEXATTRIBUTES_HPP
#define COMPUTER_GRAPHICS_ASSIGNMENTS_VERTEXATTRIBUTES_HPP

#include <glm/glm.hpp>

class VertexAttributes {
public:
  VertexAttributes() {
    posWF = {0, 0, 0};
    zinv = 0;
  };

  VertexAttributes(glm::vec3 posWF) : posWF(posWF), zinv(1/posWF.z) {}
  glm::vec3 posWF;
  float zinv;
};


#endif //COMPUTER_GRAPHICS_ASSIGNMENTS_VERTEXATTRIBUTES_HPP
