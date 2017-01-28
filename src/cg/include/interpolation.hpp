#include <vector>
#include <glm/glm.hpp>

namespace cg {
  void interpolate(float min, float max, std::vector<float>& result);
  void interpolate(glm::vec3 min, glm::vec3 max, std::vector<glm::vec3>& result);
  float rescale(float lower_bound, float upper_bound, float n);
}
