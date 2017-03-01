#ifndef RAYTRACER_DEBUG_HPP
#define RAYTRACER_DEBUG_HPP
#include <ostream>
#include <glm/glm.hpp>

namespace cg {
std::ostream& operator<<(std::ostream& os, const glm::vec3& triangle);
std::ostream& operator<<(std::ostream& os, const glm::ivec2 &v);
std::ostream &operator<<(std::ostream &os, const glm::vec2 &v);
}
#endif //RAYTRACER_DEBUG_HPP
