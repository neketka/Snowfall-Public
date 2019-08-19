#pragma once
#include <glm/glm.hpp>

float FractalSimplex(int octaves, glm::vec2 pos, float persistence, float lacunarity, float scale, float min, float max);