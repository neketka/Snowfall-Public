#include "Lighting"

#ifndef DEFERRED_LIGHTING_INCLUDE
#define DEFERRED_LIGHTING_INCLUDE
layout(std430, binding = 2) buffer LightingBuffer
{
	int PassLightCount;
	int PassLightIndices[31];
	Light AllLights[];
};
#endif