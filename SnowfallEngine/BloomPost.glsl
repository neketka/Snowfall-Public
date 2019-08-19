#include "PostProcessShader"

//Based on LearnOpenGL basic method

layout(location = 5) uniform sampler2D avgLuma;
layout(location = 6) uniform vec4 lumaValues;

#passes PASS0 PASS1_0 PASS2_0 PASS1_1 PASS2_1 PASS1_2 PASS2_2 PASS1_3 PASS2_3 PASS1_4 PASS2_4 PASS3

#downscale 0

#downsample 1 0
#downsample 2 0
#downsample 3 1
#downsample 4 1
#downsample 5 2
#downsample 6 2
#downsample 7 3
#downsample 8 3
#downsample 9 4
#downsample 10 4

#ifdef PASS1_0

#define PASS1
#define LEVEL 0

#endif

#ifdef PASS2_0

#define PASS2
#define LEVEL 0

#endif

#ifdef PASS1_1

#define PASS1
#define LEVEL 1

#endif

#ifdef PASS2_1

#define PASS2
#define LEVEL 1

#endif

#ifdef PASS1_2

#define PASS1
#define LEVEL 2

#endif

#ifdef PASS2_2

#define PASS2
#define LEVEL 2

#endif

#ifdef PASS1_3

#define PASS1
#define LEVEL 3

#endif

#ifdef PASS2_3

#define PASS2
#define LEVEL 3

#endif

#ifdef PASS1_4

#define PASS1
#define LEVEL 4

#endif

#ifdef PASS2_4

#define PASS2
#define LEVEL 4

#endif

#ifdef FRAGMENT

#ifdef PASS0
void main()
{
	NO_COLOR;

	vec4 FragColor = Snowfall_GetColor();
	float luma = clamp(texture(avgLuma, vec2(0, 0)).r, lumaValues.x, lumaValues.y) + 1;
	float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));

	vec4 BrightColor = vec4(FragColor.rgb * float(brightness > luma), 1.0);

	Snowfall_SetAuxillary(BrightColor);
}
#endif

#if defined(PASS1) || defined(PASS2)

uniform float weight[5] = float[](0.06136, 0.24477, 0.38774, 0.24477, 0.06136);

void main()
{
	vec2 tex_offset = 1.0 / Snowfall_GetResolutionLevel(LEVEL); // gets size of single texel
	vec3 result = Snowfall_GetAuxillaryLevel(LEVEL).rgb * weight[0]; // current fragment's contribution
	vec2 TexCoords = Snowfall_GetTexcoord();

	for (int i = 1; i < 5; ++i)
	{
#ifdef PASS1
		result += Snowfall_SampleAuxillaryLevel(TexCoords + vec2(tex_offset.x * i, 0.0), LEVEL).rgb * weight[i];
		result += Snowfall_SampleAuxillaryLevel(TexCoords - vec2(tex_offset.x * i, 0.0), LEVEL).rgb * weight[i];
#else
		result += Snowfall_SampleAuxillaryLevel(TexCoords + vec2(0.0, tex_offset.y * i), LEVEL).rgb * weight[i];
		result += Snowfall_SampleAuxillaryLevel(TexCoords - vec2(0.0, tex_offset.y * i), LEVEL).rgb * weight[i];
#endif
	}

	Snowfall_SetAuxillary(vec4(result, 1.0));
}
#endif

#ifdef PASS3

void main()
{
	NO_AUXILLARY;
	Snowfall_SetColor(Snowfall_GetColor() + 0.2 * 
		(Snowfall_GetAuxillaryLevel(0) + 
		 Snowfall_GetAuxillaryLevel(1) +
		 Snowfall_GetAuxillaryLevel(2) +
		 Snowfall_GetAuxillaryLevel(3) +
		 Snowfall_GetAuxillaryLevel(4)  ));
}

#endif

#endif