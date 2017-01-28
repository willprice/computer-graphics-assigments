#include "interpolation.hpp"
#include <glm/glm.hpp>
#include <iostream>

using namespace std;
using namespace glm;

namespace cg {
  void interpolate(float min, float max, vector<float> &result) {
    if (result.size() == 1) {
      result[0] = (min + max) / 2;
      return;
    }
    float step_size = (max - min) / (result.size() - 1);

    for (int i = 0; i < result.size(); i++) {
      result[i] = min + step_size * ((float)i);
    }
  }

  void interpolate(vec3 min, vec3 max, vector<vec3> &result) {
    if (result.size() == 1) {
      result[0].x = (min.x + max.x) / 2;
      result[0].y = (min.y + max.y) / 2;
      result[0].z = (min.z + max.z) / 2;
      return;
    }

    float x_step_size = (max.x - min.x) / (result.size() - 1);
    float y_step_size = (max.y - min.y) / (result.size() - 1);
    float z_step_size = (max.z - min.z) / (result.size() - 1);
    for (int i = 0; i < result.size(); i++) {
      result[i].x = min.x + x_step_size * i;
      result[i].y = min.y + y_step_size * i;
      result[i].z = min.z + z_step_size * i;
    };
    return;
  }

  float rescale(float lower_bound, float upper_bound, float n) {
    return lower_bound + n * (upper_bound - lower_bound);
  }

};
