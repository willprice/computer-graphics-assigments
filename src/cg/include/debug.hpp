#ifndef RAYTRACER_DEBUG_HPP
#define RAYTRACER_DEBUG_HPP
#include <ostream>
#include <glm/glm.hpp>

namespace cg {
std::ostream& operator<<(std::ostream& os, glm::vec3& triangle);
std::ostream& operator<<(std::ostream& os, glm::ivec2 &v);
std::ostream &operator<<(std::ostream &os, glm::vec2 &v);
}
#endif //RAYTRACER_DEBUG_HPP
