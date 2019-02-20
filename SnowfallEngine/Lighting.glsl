#ifndef LIGHTING_INCLUDE
#define LIGHTING_INCLUDE

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float DistributionGGX(float NdotH, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH2 = NdotH * NdotH;

	float nom = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;

	return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r*r) / 8.0;

	float nom = NdotV;
	float denom = NdotV * (1.0 - k) + k;

	return nom / denom;
}

float GeometrySmith(float NdotV, float NdotL, float roughness)
{
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}

float orenNayarDiffuse(float LdotV, float NdotL, float NdotV, float roughness, float albedo)
{
	float s = LdotV - NdotL * NdotV;
	float t = mix(1.0, max(NdotL, NdotV), step(0.0, s));

	float sigma2 = roughness * roughness;
	float A = 1.0 + sigma2 * (albedo / (sigma2 + 0.13) + 0.5 / (sigma2 + 0.33));
	float B = 0.45 * sigma2 / (sigma2 + 0.09);

	return albedo * max(0.0, NdotL) * (A + B * s / t) / PI;
}

vec3 BlinnLambertBRDF(vec3 N, vec3 L, vec3 H, vec3 radiance,
	vec3 diffuse, vec3 specular, float gloss)
{
	float Id = max(0, dot(N, L));
	float Is = pow(dot(N, H), gloss);

	return Id * diffuse + Is * specular;
}

vec3 WardOrenBRDF(vec3 N, vec3 V, vec3 L, vec3 H, vec3 T, vec3 B, vec3 radiance,
	vec3 diffuse, vec3 specular, float alphaX, float alphaY)
{
	float ax = alphaX;
	float ay = alphaY;

	float a = dot(H, T) / ax;
	float b = dot(H, B) / ay;
	float c = dot(H, N);

	float exponent = -(a * a + b * b) / (c * c);

	float spec = 1.0 / (4.0 * 3.14159265 * ax * ay * sqrt(dot(L, N) * dot(V, N)));
	spec *= exp(exponent);

	return diffuse / PI + specular * spec;
}

vec3 CookOrenBRDF(vec3 N, vec3 V, vec3 L, vec3 H, vec3 radiance,
	vec3 diffuse, float metalness, float roughness)
{
	float LdotV = max(dot(L, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float NdotV = max(dot(N, V), 0.0);
	float NdotH = max(dot(N, H), 0.0);
	float HdotV = max(dot(H, V), 0.0);

	vec3 F0 = mix(vec3(0.04), diffuse, metalness);
	float NDF = DistributionGGX(NdotH, roughness);
	float G = GeometrySmith(NdotV, NdotL, roughness);
	vec3 F = fresnelSchlick(HdotV, F0);

	vec3 kS = F;
	vec3 kD = vec3(1.0) - kS;
	kD *= 1.0 - metalness;

	vec3 num = NDF * G * F;
	float denom = 4.0 * NdotV * NdotL;
	vec3 specular = num / max(denom, 0.001);
	return (kD * diffuse / PI + specular) * radiance * orenNayarDiffuse(LdotV, NdotL, NdotV, roughness, 0.7);
}

float Attenutation(vec3 L, float lightRadius, float cutoff)
{
	float r = lightRadius;
	float distance = length(L);
	float d = max(distance - r, 0);

	float denom = d / r + 1;
	float attenuation = 1 / (denom*denom);

	attenuation = (attenuation - cutoff) / (1 - cutoff);
	attenuation = max(attenuation, 0);
	return attenuation;
}
#endif