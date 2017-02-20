#ifndef LIBCG_TRIANGLE_H
#define LIBCG_TRIANGLE_H
// This code originates from TestModel.h provided by Carl Henrik Ek
#include <glm/glm.hpp>

namespace cg {
  class Triangle {
    public:
      glm::vec3 v0;
      glm::vec3 v1;
      glm::vec3 v2;
      glm::vec3 normal;
      glm::vec3 color;
      bool mirror = false;

      Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 color);
      Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 color, bool mirror);

      void ComputeNormal();

      glm::vec3 e1();
      glm::vec3 e2();

  };
};

#endif
