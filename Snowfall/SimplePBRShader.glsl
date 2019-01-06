#include "SnowfallShader"
#include "Lighting"

#ifdef FRAGMENT

layout(location = 0) out vec4 fragment;
layout(location = 2) uniform sampler2D tex;

void main()
{
	vec3 lightPos = vec3(0.0, 1.0, 0.0);
	vec3 camPos = vec3(0.0, 0.0, 0.0);
	vec3 fragPos = Snowfall_GetPosition();

	float dist = length(fragPos - lightPos);
	float att = 1.0 / (dist * dist);
	vec4 params = Snowfall_GetObjectParameter(0);

	vec3 radiance = vec3(50.0, 50.0, 50.0) * att;
	vec3 Kd = texture(tex, Snowfall_GetTexcoord() * 2.0).rgb;

	vec3 L = normalize(fragPos - lightPos);
	vec3 V = normalize(fragPos - camPos);
	vec3 N = normalize(Snowfall_GetNormal());
	vec3 H = normalize(L + V);

	vec3 lighting = CookOrenBRDF(N, V, L, H, radiance, Kd, params.x, params.y);

	fragment = vec4(lighting + vec3(0.03), 1);
}

#endif