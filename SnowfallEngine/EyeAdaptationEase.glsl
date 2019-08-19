#ifdef VERTEX

layout(location = 0) in vec3 Position;
layout(location = 0) out vec2 Texcoord;

void main()
{
	Texcoord = Position.xy * 0.5 + 0.5;
	gl_Position = vec4(Position, 1);
}

#endif

#ifdef FRAGMENT

layout(location = 0) out vec4 fragment;
layout(location = 0) in vec2 Texcoord;

layout(location = 0) uniform sampler2D avg;
layout(location = 1) uniform sampler2D _new;
layout(location = 2) uniform vec4 constants; //min, max, deltaT, easeConstant
layout(location = 3) uniform int lod;

void main()
{
	float avgT = texture(avg, vec2(0)).r;
	float newT = exp(textureLod(_new, vec2(0), float(lod)).r);

	avgT += (newT - avgT) * (1 - exp(-constants.z * constants.w));

	fragment = vec4(avgT);
}

#endif