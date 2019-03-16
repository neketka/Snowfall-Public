#include "SnowfallShader"

#ifdef FRAGMENT

layout(location = 15) uniform sampler2D tex;

void main()
{
	vec4 params = Snowfall_GetObjectParameter(0);
	
	Material mat;

	mat.Diffuse = texture(tex, Snowfall_GetTexcoord()).rgb;
	mat.Normal = Snowfall_GetNormal();
	mat.Metalness = params.x;
	mat.Roughness = params.y;
	mat.AO = 1;

	Snowfall_SetMaterialData(mat);
}

#endif