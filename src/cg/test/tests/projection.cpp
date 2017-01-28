#include "projection.hpp"
#include "catch.hpp"
#include "glm/common.hpp"
#include <vector>

using namespace std;
using namespace glm;
using namespace cg;

TEST_CASE("Projection", "[projection][2d][3d]") {
  int width = 500;
  int height = 500;
  Uint32 rmask, gmask, bmask, amask;
  rmask = 0xff000000;
  gmask = 0x00ff0000;
  bmask = 0x0000ff00;
  amask = 0x000000ff;
  float focal_length = 10;

  SDL_Surface *screen =
      SDL_CreateRGBSurface(0, width, height, 32, rmask, gmask, bmask, amask);

  vec3 point;
  vec2 expected;
  vec2 actual;

  SECTION("Projecting points lying on image plane") {
    point = vec3(0, 0, focal_length);
    expected = vec2(width / 2, height / 2);
    actual = project(screen, point, focal_length);

    REQUIRE(expected.x == Approx(actual.x));
    REQUIRE(expected.y == Approx(actual.y));

    point = vec3(10, 10, focal_length);
    expected = vec2(10 + width / 2, 10 + height / 2);
    actual = project(screen, point, focal_length);

    REQUIRE(expected.x == Approx(actual.x));
    REQUIRE(expected.y == Approx(actual.y));
  }

  SECTION("Projecting points not on the image plane") {
    point = vec3(0, 0, 2 * focal_length);
    expected = vec2(width / 2, height / 2);
    actual = project(screen, point, focal_length);

    REQUIRE(expected.x == Approx(actual.x));
    REQUIRE(expected.y == Approx(actual.y));

    point = vec3(50, 50, 2 * focal_length);
    expected = vec2(25 + width / 2, 25 + height / 2);
    actual = project(screen, point, focal_length);

    REQUIRE(expected.x == Approx(actual.x));
    REQUIRE(expected.y == Approx(actual.y));
  }
}
