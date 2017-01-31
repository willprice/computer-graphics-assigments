#ifndef LIBCG_TRIANGLE_H
#define LIBCG_TRIANGLE_H
// This code originates from TestModel.h provided by Carl Henrik Ek
#include <glm/glm.hpp>

namespace cg {
  class Triangle {
    public:
      glm::vec4 v0;
      glm::vec4 v1;
      glm::vec4 v2;
      glm::vec4 normal;
      glm::vec3 color;

      Triangle(glm::vec4 v0, glm::vec4 v1, glm::vec4 v2, glm::vec3 color);
      void ComputeNormal();
  };
};

#endif
