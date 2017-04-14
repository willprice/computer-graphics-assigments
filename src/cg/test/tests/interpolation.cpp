#include "interpolation.hpp"
#include "../catch.hpp"
#include "glm/ext.hpp"
#include <glm/glm.hpp>

using namespace std;
using namespace glm;
using namespace cg;

TEST_CASE("Linear interpolation of floats", "[util][interpolation]") {
  vector<float> actual(10);
  vector<float> expected(10);

  SECTION("1 to 1, in steps of 1") {
    actual.resize(1);
    expected.resize(1);
    expected = {1};

    interpolate(1, 1, actual);

    REQUIRE(expected.size() == actual.size());
    for (int i = 0; i < expected.size(); i++) {
      REQUIRE(expected[i] == Approx(actual[i]));
    }
  }

  SECTION("1 to 10 in steps of 1") {
    expected = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    interpolate(1, 10, actual);

    REQUIRE(expected == actual);
  }

  SECTION("1 to 5.5 in steps of 0.5") {
    expected = {1, 1.5, 2, 2.5, 3, 3.5, 4, 4.5, 5, 5.5};

    interpolate(1, 5.5, actual);

    REQUIRE(expected == actual);
  }

  SECTION("1 to 3 in 1 step") {
    expected = {2};
    actual.resize(1);

    interpolate(1, 3, actual);

    REQUIRE(expected == actual);
  }
}

TEST_CASE("Linear interpolation of vec3", "[util][interpolation]") {
  vector<vec3> actual(10);
  vector<vec3> expected(10);
  vec3 min;
  vec3 max;

  SECTION("(1, 1, 1) to (1, 1, 1), with 1 step") {
    min = vec3(1, 1, 1);
    max = vec3(1, 1, 1);
    expected = {min};

    actual.resize(1);
    interpolate(min, max, actual);

    REQUIRE(expected == actual);
  };

  SECTION("(0, 0, 0) to (1, 0, 0), with 11 steps") {
    expected = {vec3(0, 0, 0),   vec3(0.1, 0, 0), vec3(0.2, 0, 0),
                vec3(0.3, 0, 0), vec3(0.4, 0, 0), vec3(0.5, 0, 0),
                vec3(0.6, 0, 0), vec3(0.7, 0, 0), vec3(0.8, 0, 0),
                vec3(0.9, 0, 0), vec3(1, 0, 0)};
    min = vec3(0, 0, 0);
    max = vec3(1, 0, 0);
    actual.resize(11);

    interpolate(min, max, actual);

    REQUIRE(expected.size() == actual.size());
    for (int i = 0; i < expected.size(); i++) {
      REQUIRE(expected[i].x == Approx(actual[i].x));
      REQUIRE(expected[i].y == Approx(actual[i].y));
      REQUIRE(expected[i].z == Approx(actual[i].z));
    }
  };

  SECTION("(0, 0, 0) to (0, 1, 0), with 11 steps") {
    expected = {vec3(0, 0, 0),   vec3(0, 0.1, 0), vec3(0, 0.2, 0),
                vec3(0, 0.3, 0), vec3(0, 0.4, 0), vec3(0, 0.5, 0),
                vec3(0, 0.6, 0), vec3(0, 0.7, 0), vec3(0, 0.8, 0),
                vec3(0, 0.9, 0), vec3(0, 1, 0)};
    min = vec3(0, 0, 0);
    max = vec3(0, 1, 0);
    actual.resize(11);

    interpolate(min, max, actual);

    REQUIRE(expected.size() == actual.size());
    for (int i = 0; i < expected.size(); i++) {
      REQUIRE(expected[i].x == Approx(actual[i].x));
      REQUIRE(expected[i].y == Approx(actual[i].y));
      REQUIRE(expected[i].z == Approx(actual[i].z));
    }
  };

  SECTION("(0, 0, 0) to (0, 0, 1), with 11 steps") {
    expected = {vec3(0, 0, 0),   vec3(0, 0, 0.1), vec3(0, 0, 0.2),
                vec3(0, 0, 0.3), vec3(0, 0, 0.4), vec3(0, 0, 0.5),
                vec3(0, 0, 0.6), vec3(0, 0, 0.7), vec3(0, 0, 0.8),
                vec3(0, 0, 0.9), vec3(0, 0, 1)};
    min = vec3(0, 0, 0);
    max = vec3(0, 0, 1);
    actual.resize(11);

    interpolate(min, max, actual);

    REQUIRE(expected.size() == actual.size());
    for (int i = 0; i < expected.size(); i++) {
      REQUIRE(expected[i].x == Approx(actual[i].x));
      REQUIRE(expected[i].y == Approx(actual[i].y));
      REQUIRE(expected[i].z == Approx(actual[i].z));
    }
  };

  SECTION("(0, 0, 0) to (1, 1, 1), with 11 steps") {
    expected = {vec3(0, 0, 0),       vec3(0.1, 0.1, 0.1), vec3(0.2, 0.2, 0.2),
                vec3(0.3, 0.3, 0.3), vec3(0.4, 0.4, 0.4), vec3(0.5, 0.5, 0.5),
                vec3(0.6, 0.6, 0.6), vec3(0.7, 0.7, 0.7), vec3(0.8, 0.8, 0.8),
                vec3(0.9, 0.9, 0.9), vec3(1, 1, 1)};
    min = vec3(0, 0, 0);
    max = vec3(1, 1, 1);
    actual.resize(11);

    interpolate(min, max, actual);

    REQUIRE(expected.size() == actual.size());
    for (int i = 0; i < expected.size(); i++) {
      REQUIRE(expected[i].x == Approx(actual[i].x));
      REQUIRE(expected[i].y == Approx(actual[i].y));
      REQUIRE(expected[i].z == Approx(actual[i].z));
    }
  };

  SECTION("(0, 0, 0) to (1, 1, 1), with 0 steps") {
    expected = {vec3(0.5, 0.5, 0.5)};
    min = vec3(0, 0, 0);
    max = vec3(1, 1, 1);
    actual.resize(1);

    interpolate(min, max, actual);

    REQUIRE(expected.size() == actual.size());
    for (int i = 0; i < expected.size(); i++) {
      REQUIRE(expected[i].x == Approx(actual[i].x));
      REQUIRE(expected[i].y == Approx(actual[i].y));
      REQUIRE(expected[i].z == Approx(actual[i].z));
    }
  };

  SECTION("(1, 4, 9.2) to (4, 1, 9.8), with 4 steps") {
    expected = {vec3(1, 4, 9.2), vec3(2, 3, 9.4), vec3(3, 2, 9.6),
                vec3(4, 1, 9.8)};
    min = vec3(1, 4, 9.2);
    max = vec3(4, 1, 9.8);
    actual.resize(4);

    interpolate(min, max, actual);

    REQUIRE(expected.size() == actual.size());
    for (int i = 0; i < expected.size(); i++) {
      REQUIRE(expected[i].x == Approx(actual[i].x));
      REQUIRE(expected[i].y == Approx(actual[i].y));
      REQUIRE(expected[i].z == Approx(actual[i].z));
    }
  };
};
