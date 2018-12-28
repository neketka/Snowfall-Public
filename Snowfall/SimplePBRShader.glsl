#include "SnowfallShader"

#ifdef FRAGMENT

layout(location = 0) out vec4 fragment;

void main()
{
	vec3 lightPos = vec3(0.0, 1.0, 2.0);
	vec3 camPos = vec3(0.0, 0.0, 0.0);
	vec3 fragPos = Snowfall_GetPosition();
	vec4 Kd = vec4(1.0, 1.0, 1.0, 1.0);
	vec4 Ks = vec4(1.0, 1.0, 1.0, 1.0);

	float dist = length(fragPos - lightPos);
	float att = 1.0 / (dist * dist);

	vec3 L = normalize(lightPos - fragPos);
	vec3 V = normalize(camPos - fragPos);
	vec3 N = normalize(Snowfall_GetNormal());
	vec3 H = normalize(L + V);

	float Id = max(0, dot(N, L));
	float Is = pow(dot(N, H), 64.0);

	fragment = (Id * Kd + Is * Ks) * att;
}

#endif