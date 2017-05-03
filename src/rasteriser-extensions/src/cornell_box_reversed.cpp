#include "cornell_box_reversed.hpp"

using cg::Triangle;

void LoadTestModel(std::vector<Triangle> &triangles) {
  using glm::vec3;
  using glm::vec2;

  // Defines colors:
  vec3 red(0.75f, 0.15f, 0.15f);
  vec3 yellow(0.75f, 0.75f, 0.15f);
  vec3 green(0.15f, 0.75f, 0.15f);
  vec3 cyan(0.15f, 0.75f, 0.75f);
  vec3 blue(0.15f, 0.15f, 0.75f);
  vec3 purple(0.75f, 0.15f, 0.75f);
  vec3 white(0.75f, 0.75f, 0.75f);
  vec3 persianRed(0.84f,0.16f,0.18f);
  vec3 hippieGreen(0.36f,0.55f,0.25f);

  triangles.clear();
  triangles.reserve(5 * 2 * 3);

  // ---------------------------------------------------------------------------
  // Room

  float L = 555; // Length of Cornell Box side.

  vec3 A(L, 0, 0);
  vec3 B(0, 0, 0);
  vec3 C(L, 0, L);
  vec3 D(0, 0, L);

  vec3 E(L, L, 0);
  vec3 F(0, L, 0);
  vec3 G(L, L, L);
  vec3 H(0, L, L);

  // Floor:
  triangles.push_back(Triangle(C, B, A, white));
  triangles.push_back(Triangle(C, D, B, white));

  // Left wall
  triangles.push_back(Triangle(A, E, C, persianRed));
  triangles.push_back(Triangle(C, E, G, persianRed));

  // Right wall
  triangles.push_back(Triangle(F, B, D, hippieGreen));
  triangles.push_back(Triangle(H, F, D, hippieGreen));

  // Ceiling
  triangles.push_back(Triangle(E, F, G, white));
  triangles.push_back(Triangle(F, H, G, white));

  // Back wall
  vec2 TL(0, 0);
  vec2 TR(1884, 0);
  vec2 BL(0, 1064);
  vec2 BR(1884, 1064);
  triangles.push_back(Triangle(G, D, C, white,false,true,TR,BL,BR));
  triangles.push_back(Triangle(G, H, D, white,false,true,TR,TL,BL));

  // ---------------------------------------------------------------------------
  // Short block

  A = vec3(290, 0, 114);
  B = vec3(130, 0, 65);
  C = vec3(240, 0, 272);
  D = vec3(82, 0, 225);

  E = vec3(290, 165, 114);
  F = vec3(130, 165, 65);
  G = vec3(240, 165, 272);
  H = vec3(82, 165, 225);

  // Front
  triangles.push_back(Triangle(E, B, A, white));
  triangles.push_back(Triangle(E, F, B, white));

  // Front
 triangles.push_back(Triangle(F, D, B, white));
 triangles.push_back(Triangle(F, H, D, white));

  // BACK
  triangles.push_back(Triangle(H, C, D, white));
  triangles.push_back(Triangle(H, G, C, white));

  // LEFT
  triangles.push_back(Triangle(G, E, C, white));
  triangles.push_back(Triangle(E, A, C, white));

  // TOP
  triangles.push_back(Triangle(G, F, E, white));
  triangles.push_back(Triangle(G, H, F, white));

  // ---------------------------------------------------------------------------
  // Tall block

  A = vec3(423, 0, 247);
  B = vec3(265, 0, 296);
  C = vec3(472, 0, 406);
  D = vec3(314, 0, 456);

  E = vec3(423, 330, 247);
  F = vec3(265, 330, 296);
  G = vec3(472, 330, 406);
  H = vec3(314, 330, 456);

  // Front
  triangles.push_back(Triangle(E, B, A, white));
  triangles.push_back(Triangle(E, F, B, white));

  // Front
  triangles.push_back(Triangle(F, D, B, white));
  triangles.push_back(Triangle(F, H, D, white));

  // BACK
  triangles.push_back(Triangle(H, C, D, white));
  triangles.push_back(Triangle(H, G, C, white));

  // LEFT
  triangles.push_back(Triangle(G, E, C, white));
  triangles.push_back(Triangle(E, A, C, white));

  // TOP
  triangles.push_back(Triangle(G, F, E, white));
  // Scale to the volume [-1,1]^3

  for (size_t i = 0; i < triangles.size(); ++i) {
    triangles[i].v0.position *= 2 / L;
    triangles[i].v1.position *= 2 / L;
    triangles[i].v2.position *= 2 / L;

    triangles[i].v0.position -= vec3(1, 1, 1);
    triangles[i].v1.position -= vec3(1, 1, 1);
    triangles[i].v2.position -= vec3(1, 1, 1);

    triangles[i].v0.position.x *= -1;
    triangles[i].v1.position.x *= -1;
    triangles[i].v2.position.x *= -1;

    triangles[i].v0.position.y *= -1;
    triangles[i].v1.position.y *= -1;
    triangles[i].v2.position.y *= -1;

    triangles[i].v0.position.z *= -1;
    triangles[i].v1.position.z *= -1;
    triangles[i].v2.position.z *= -1;

    triangles[i].ComputeNormal();
  }
}
