#include "Lighting"

#ifndef DEFERRED_LIGHTING_INCLUDE
#define DEFERRED_LIGHTING_INCLUDE

#define LIGHT_CASCADES 4

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
	float ShadowCascades[LIGHT_CASCADES];
	int CascadeIndices[LIGHT_CASCADES];
	//int Padding1[mod(LIGHT_CASCADES, 2)];
	mat4 CascadeMatrices[LIGHT_CASCADES];

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

	float dist = length(data.CameraPosition - data.PixelPosition);

	const int SHADOW_SAMPLES = 9;
	const int DIRSHADOW_SAMPLES = 16;

	const float SHADOW_SPAN = sqrt(SHADOW_SAMPLES) / 2.0;
	const float DIRSHADOW_SPAN = sqrt(SHADOW_SAMPLES) / 2.0;

	ivec2 offset = ivec2(2, 2);
	vec4 points = vec4(1.0, 1.0, 1.0, 1.0);
	vec4 shadowCoord;

	vec2 flatTexels = 1.0 / textureSize(FlatShadows, 0).xy;
	vec2 cubeTexels = 1.0 / textureSize(CubeShadows, 0).xy;
	vec2 dirTexels = 1.0 / textureSize(HighDirectionalShadow, 0);

	float bias, dn;
	int index = 0;

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
			shadow = 0.0;

			dn = dot(data.Normal, -light.Direction.xyz);
			bias = 0.005 * sqrt(1 - dn * dn) / dn;

			switch (light.Type)
			{
			case 0:
				mat4 lightSpace;
				for (int i = 0; i < LIGHT_CASCADES; ++i)
				{
					if (dist < ShadowCascades[i])
					{
						index = CascadeIndices[i];
						lightSpace = CascadeMatrices[i];
						break;
					}
				}
				shadowCoord = lightSpace * vec4(data.PixelPosition, 1);
				shadowCoord /= shadowCoord.w;
				for (float y = -SHADOW_SPAN; y < SHADOW_SPAN; y += 1.0)
					for (float x = -SHADOW_SPAN; x < SHADOW_SPAN; x += 1.0)
						shadow += texture(FlatShadows, vec4(shadowCoord.xy + vec2(x, y) * flatTexels, index, shadowCoord.z - bias));
				shadow /= SHADOW_SAMPLES;
				break;
			case 1:
				//shadow = texture(CubeShadows, );
				break;
			case 2:
				shadowCoord = light.LightSpace * vec4(data.PixelPosition, 1);
				shadowCoord /= shadowCoord.w;
				for (float y = -SHADOW_SPAN; y < SHADOW_SPAN; y += 1.0)
					for (float x = -SHADOW_SPAN; x < SHADOW_SPAN; x += 1.0)
						shadow += texture(FlatShadows, vec4(shadowCoord.xy + vec2(x, y) * flatTexels * shadowCoord.w, light.HighIndex, shadowCoord.z - bias));
				shadow /= SHADOW_SAMPLES;
				break;
			}
		}
		color += CalculateLight(data.CameraPosition, data.PixelPosition, light, mat, shadow);
	}
	return color + mat.Emissive;
}

#endif