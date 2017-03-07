#include <glm/glm.hpp>
#include <vector>

namespace cg {
void interpolate(float min, float max, std::vector<float> &result);
void interpolate(glm::vec3 min, glm::vec3 max, std::vector<glm::vec3> &result);
void interpolate(glm::ivec2 a, glm::ivec2 b, std::vector<glm::ivec2> &result);
float rescale(float lower_bound, float upper_bound, float n);
}
