#ifndef CORNELL_BOX_H
#define CORNELL_BOX_H
// Defines a simple test model: The Cornell Box
// Author: Carl Henrik Ek
// Origins: Provided as sample code for a ray tracer in COMS30115

#include "triangle.hpp"
#include <glm/glm.hpp>
#include <vector>

// Loads the Cornell Box. It is scaled to fill the volume:
// -1 <= x <= +1
// -1 <= y <= +1
// -1 <= z <= +1
void LoadTestModel(std::vector<cg::Triangle> &triangles);

#endif
