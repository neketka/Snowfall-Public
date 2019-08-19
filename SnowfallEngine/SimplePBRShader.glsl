#include "SnowfallShader"

#ifdef MATERIAL

layout(location = 20) uniform sampler2D tex;

void MaterialProps(inout Material mat, PerVertexData data)
{
	vec4 params = Snowfall_GetObjectParameter(0);

	float t = Snowfall_GetTime();

	vec2 coord = Snowfall_GetTexcoord();

	mat.Diffuse = texture(tex, coord).rgb;
	mat.Normal = Snowfall_GetNormal();
	mat.Metalness = params.x;
	mat.Roughness = params.y;
	mat.AO = 1.0;
	mat.Alpha = 1.0;
}

#endif