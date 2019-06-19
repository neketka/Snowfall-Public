#include "SnowfallShader"

#ifdef FRAGMENT

layout(location = 16) uniform sampler2D tex;

void main()
{
	vec4 params = Snowfall_GetObjectParameter(0);
	
	Material mat;
	float t = Snowfall_GetTime();

	vec2 coord = Snowfall_GetTexcoord();

	mat.Diffuse = texture(tex, coord).rgb;
	mat.Normal = Snowfall_GetNormal();
	mat.Metalness = params.x;
	mat.Roughness = params.y;
	mat.AO = 1.0;

	Snowfall_SetMaterialData(mat);
}

#endif