#include "PostProcessShader"

#passes PASS0 PASS1

#ifdef FRAGMENT

#if defined(PASS0) || defined(PASS1)

uniform vec3 weight[9] = vec3[9](
	vec3(0.0000000000000000000, 0.04416589065853191, 0.0922903086524308425), vec3(0.10497808951021347), vec3(0.0922903086524308425, 0.04416589065853191, 0.0000000000000000000),
	vec3(0.0112445223775533675, 0.10497808951021347, 0.1987116566428735725), vec3(0.40342407932501833), vec3(0.1987116566428735725, 0.10497808951021347, 0.0112445223775533675),
	vec3(0.0000000000000000000, 0.04416589065853191, 0.0922903086524308425), vec3(0.10497808951021347), vec3(0.0922903086524308425, 0.04416589065853191, 0.0000000000000000000)
);

void main()
{
	vec2 tex_offset = 1.0 / Snowfall_GetResolution(); // gets size of single texel
	vec3 result = Snowfall_GetColor().rgb * weight[0]; // current fragment's contribution
	vec2 TexCoords = Snowfall_GetTexcoord();

	for (int i = 1; i < 9; ++i)
	{
#ifdef PASS0
		result += Snowfall_SampleColor(TexCoords + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
		result += Snowfall_SampleColor(TexCoords - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
#else
		result += Snowfall_SampleColor(TexCoords + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
		result += Snowfall_SampleColor(TexCoords - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
#endif
	}

	Snowfall_SetColor(vec4(result, 1.0));
}
#endif

#endif