#include <vector>
#include <glm/glm.hpp>

namespace cg {
  void interpolate(float min, float max, std::vector<float>& result);
  void interpolate(glm::vec4 min, glm::vec4 max, std::vector<glm::vec4>& result);
  float rescale(float lower_bound, float upper_bound, float n);
}
