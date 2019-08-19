#ifndef TERRAIN_SHADER_INCLUDE
#define TERRAIN_SHADER_INCLUDE

#define CUSTOM_VERTEX
#define OVERRIDE_ATTRIBUTES
#include "SnowfallShader"
#include "Utilities"

#ifdef VERTEX

layout(location = 0) in vec2 UV;

layout(location = 16) uniform sampler2D TerrainMap;
layout(location = 17) uniform sampler2D NormalMap;
layout(location = 18) uniform vec4 WorldOffsets;
layout(location = 19) uniform vec4 TextureOffsets;

const vec3 MyArray[4] = vec3[4](
	vec3(0, 0, 0),
	vec3(1, 0, 0),
	vec3(0, 1, 0),
	vec3(0, 0, 1)
);

void main()
{
	vec4 terrain = textureLod(TerrainMap, UV * TextureOffsets.x, WorldOffsets.z); // (height, alphamap, unused, unused)
	vec4 normal = textureLod(NormalMap, UV * TextureOffsets.x, WorldOffsets.z); // (normalTheta, normalPhi, tangentTheta, tangentPhi)
	
	VertexOutputData data;

	data.Position = vec3(WorldOffsets.x, 0, WorldOffsets.y) + vec3(UV.x, 0, UV.y) * (WorldOffsets.w) + vec3(0, terrain.r, 0);
	data.Color = unpackUnorm4x8(floatBitsToUint(terrain.g));
	data.Normal = SphericalToCartesianNormal(normal.rg);
	data.Tangent = SphericalToCartesianNormal(normal.ba);
	data.Texcoord = UV;

	Snowfall_SetOutputData(data);
}

#endif

#endif