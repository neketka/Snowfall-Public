#include "Lighting"

#ifndef DEFERRED_LIGHTING_INCLUDE
#define DEFERRED_LIGHTING_INCLUDE

struct PerVertexData
{
	vec3 CameraPosition;
	vec3 PixelPosition;
	vec2 Texcoord;
	vec3 Normal;
	vec3 Tangent;
	vec4 Color;
	mat3 TBNMatrix;
};

layout(std430, binding = 2) buffer LightingBuffer
{
	int PassLightCount;
	int PassLightIndices[31];
	Light AllLights[];
};

layout(location = 12) uniform sampler2DShadow HighDirectionalShadow;
layout(location = 13) uniform sampler2DArrayShadow FlatShadows;
layout(location = 14) uniform samplerCubeArrayShadow CubeShadows;

vec3 Snowfall_GetMaterialResult(Material mat, PerVertexData data)
{
	vec3 color = vec3(0.0, 0.0, 0.0);

	const int DIRECTIONAL_SHADOW_SAMPLES = 16;
	const int SHADOW_SAMPLES = 9;

	const float DIRECTIONAL_SHADOW_SPAN = sqrt(DIRECTIONAL_SHADOW_SAMPLES) / 2.0;
	const float SHADOW_SPAN = sqrt(SHADOW_SAMPLES) / 2.0;

	for (int i = 0; i < PassLightCount; ++i)
	{
		Light light = AllLights[PassLightIndices[i]];
		float shadow = 1.0;

		if (allInvocations(light.HighIndex == -1))
		{
			shadow = 1.0;
		}
		else
		{
			ivec2 offset = ivec2(2, 2);
			vec4 points = vec4(1.0, 1.0, 1.0, 1.0);
			vec4 shadowCoord = light.LightSpace * vec4(data.PixelPosition, 1);

			vec2 flatTexels = 1.0 / textureSize(FlatShadows, 0).xy;
			vec2 cubeTexels = 1.0 / textureSize(CubeShadows, 0).xy;
			vec2 dirTexels = 1.0 / textureSize(HighDirectionalShadow, 0);

			shadow = 0.0;

			switch (light.Type)
			{
			case 0:
				for (float y = -DIRECTIONAL_SHADOW_SPAN; y < DIRECTIONAL_SHADOW_SPAN; y += 1.0)
					for (float x = -DIRECTIONAL_SHADOW_SPAN; x < DIRECTIONAL_SHADOW_SPAN; x += 1.0)
						shadow += texture(HighDirectionalShadow, vec3(shadowCoord.xy + vec2(x, y) * dirTexels, shadowCoord.z));
				shadow /= DIRECTIONAL_SHADOW_SAMPLES;
				break;
			case 1:
				//shadow = texture(CubeShadows, );
				break;
			case 2:
				shadowCoord /= shadowCoord.w;
				for (float y = -SHADOW_SPAN; y < SHADOW_SPAN; y += 1.0)
					for (float x = -SHADOW_SPAN; x < SHADOW_SPAN; x += 1.0)
						shadow += texture(FlatShadows, vec4(shadowCoord.xy + vec2(x, y) * flatTexels * shadowCoord.w, light.HighIndex, shadowCoord.z));
				shadow /= SHADOW_SAMPLES;
				break;
			}
		}
		color += CalculateLight(data.CameraPosition, data.PixelPosition, light, mat, shadow);
	}
	return color + mat.Emissive;
}

#endif