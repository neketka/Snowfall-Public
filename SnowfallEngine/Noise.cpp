#include "stdafx.h"
#include "Noise.h"
#include <glm/gtc/noise.hpp>

float FractalSimplex(int octaves, glm::vec2 pos, float persistence, float lacunarity, float scale, float min, float max)
{
	float maxAmp = 0;
	float amp = 1;
	float freq = scale;
	float noise = 0;

	for (int i = 0; i < octaves; ++i)
	{
		noise += glm::simplex(pos * freq) * amp;
		maxAmp += amp;
		amp *= persistence;
		freq *= lacunarity;
	}

	noise /= maxAmp;

	noise = noise * (max - min) / 2 + (max + min) / 2;

	return noise;
}
