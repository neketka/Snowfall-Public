#include "SnowfallShader"

#ifdef FRAGMENT

layout(location = 16) uniform sampler2D tex;

void main()
{
	vec4 params = Snowfall_GetObjectParameter(0);
	
	Material mat;
	float t = Snowfall_GetTime();

	vec2 coord = Snowfall_GetTexcoord();

	float calc = float(coord.x <= 0.55 && coord.x >= 0.45) * mix(1.1, 3.3, sin(t * 2) * 0.5 + 0.5);

	mat.Diffuse = texture(tex, coord).rgb;
	mat.Normal = Snowfall_GetNormal();
	mat.Metalness = params.x;
	mat.Roughness = params.y;
	mat.AO = 1.0;
	mat.Emissive = vec3(1.0, 0.84, 0.0) * calc;

	Snowfall_SetMaterialData(mat);
}

#endif