#include "TerrainShader"

#ifdef MATERIAL

layout(location = 20) uniform sampler2D SnowDiffuse;
layout(location = 21) uniform sampler2D SnowTangentMap;
layout(location = 22) uniform sampler2D SnowRoughness;
layout(location = 23) uniform sampler2D SnowAO;

layout(location = 24) uniform sampler2D GroundDiffuse;
layout(location = 25) uniform sampler2D GroundTangentMap;
layout(location = 26) uniform sampler2D GroundRoughness;
layout(location = 27) uniform sampler2D GroundAO;

void MaterialProps(inout Material mat, PerVertexData data)
{
	float t = Snowfall_GetTime();

	vec2 coord = data.Texcoord * 16.0;
	vec4 alpha = data.Color;

	mat.Diffuse = texture(SnowDiffuse, coord).rgb * alpha.r + texture(GroundDiffuse, coord).rgb * alpha.g;
	mat.Normal = data.TBNMatrix * (texture(SnowTangentMap, coord).rgb * alpha.r + texture(GroundTangentMap, coord).rgb * alpha.g);
	mat.Metalness = 0;
	mat.Roughness = texture(SnowRoughness, coord).r * alpha.r + texture(GroundRoughness, coord).r * alpha.g;
	mat.AO = texture(SnowAO, coord).r * alpha.r + texture(GroundAO, coord).r * alpha.g;
}

#endif