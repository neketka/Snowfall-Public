#include "SnowfallShader"
#include "Lighting"

#ifdef FRAGMENT

layout(location = 0) out vec4 fragment;
layout(location = 4) uniform sampler2D tex;

void main()
{
	vec3 lightPos = vec3(0.0, 0.0, 0.0);
	vec3 camPos = vec3(0.0, 0.0, 0.0);
	vec3 fragPos = Snowfall_GetPosition();

	vec4 params = Snowfall_GetObjectParameter(0);

	vec3 radiance = vec3(50.0, 50.0, 50.0) * Attenutation(lightPos - fragPos, 1.5, 0.005);
	vec3 Kd = texture(tex, Snowfall_GetTexcoord()).rgb;

	vec3 L = normalize(lightPos - fragPos);
	vec3 V = normalize(camPos - fragPos);
	vec3 N = normalize(Snowfall_GetNormal());
	vec3 H = normalize(L + V);

	vec3 lighting = CookOrenBRDF(N, V, L, H, radiance, Kd, params.x, params.y);

	fragment = vec4(lighting + 0.03, 1);
}

#endif