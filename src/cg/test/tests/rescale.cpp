#include "../catch.hpp"

#include "glm/ext.hpp"
#include "interpolation.hpp"
#include <glm/glm.hpp>

using namespace std;
using namespace glm;
using namespace cg;

TEST_CASE("Rescaling", "[rescale]"){
    SECTION("Rescaling values in [0, 1] yields the same number"){
        for (float n = 0; n < 1;
             n += 0.1){REQUIRE(n == Approx(rescale(0, 1, n)));
}
}

SECTION("Rescaling values to [0, 2] yields the double the input") {
  for (float n = 0; n < 1; n += 0.1) {
    REQUIRE(n * 2 == Approx(rescale(0, 2, n)));
  }
}
SECTION("Rescaling values to [1, 3] yields the double the input + 1") {
  for (float n = 0; n < 1; n += 0.1) {
    REQUIRE((1 + n * 2) == Approx(rescale(1, 3, n)));
  }
}
SECTION("Rescaling values to [50, 100] yields 50* the input + 50") {
  for (float n = 0; n < 1; n += 0.1) {
    REQUIRE((50 + n * 50) == Approx(rescale(50, 100, n)));
  }
}
}
;
