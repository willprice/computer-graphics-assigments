#ifndef COMPUTER_GRAPHICS_ASSIGNMENTS_VERTEXATTRIBUTES_HPP
#define COMPUTER_GRAPHICS_ASSIGNMENTS_VERTEXATTRIBUTES_HPP

#include <glm/glm.hpp>

class VertexAttributes {
public:
  VertexAttributes() {
    posWF = {0, 0, 0};
    zinv = 0;
    texturePosition = {0, 0};
    vertexNum = -1;
  };

  VertexAttributes(glm::vec3 posWF, glm::vec2 texturePosition, int vertexNum) : posWF(posWF), zinv(1/posWF.z), texturePosition(texturePosition), vertexNum(vertexNum) {}
  glm::vec3 posWF;
  glm::vec2 texturePosition;
  int vertexNum;
  float zinv;
};


#endif //COMPUTER_GRAPHICS_ASSIGNMENTS_VERTEXATTRIBUTES_HPP
