#include <glm/glm.hpp>
#include <iostream>

using namespace std;

namespace glm {
std::ostream &operator<<(std::ostream &out, const glm::vec3 &vec) {
  out << "{" << vec.x << " " << vec.y << " " << vec.z << "}";
  return out;
}
};
