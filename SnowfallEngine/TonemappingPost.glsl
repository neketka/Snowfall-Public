#include "PostProcessShader"

#passes PASS0

layout(location = 5) uniform sampler2D avgLuma;
layout(location = 6) uniform vec4 lumaValues;

// Based on Filmic Tonemapping Operators http://filmicgames.com/archives/75
vec3 tonemapFilmic(vec3 color) 
{
	vec3 x = max(vec3(0.0), color - 0.004);
	return (x * (6.2 * x + 0.5)) / (x * (6.2 * x + 1.7) + 0.06);
}

// https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/
vec3 acesFilm(vec3 x) 
{
	const float a = 2.51;
	const float b = 0.03;
	const float c = 2.43;
	const float d = 0.59;
	const float e = 0.14;
	return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

vec3 tonemapReinhard(vec3 color) 
{
	return color / (color + vec3(1.0));
}

#ifdef FRAGMENT

#ifdef PASS0

void main()
{
	NO_AUXILLARY;
	float luma = texture(avgLuma, vec2(0, 0)).r;
	float exposure = lumaValues.w / (clamp(luma, lumaValues.x, lumaValues.y) - lumaValues.z);

	vec4 sampled = Snowfall_GetColor();

	vec3 color = acesFilm(sampled.xyz * exposure);

	Snowfall_SetColor(vec4(color, sampled.a));
}

#endif

#endif