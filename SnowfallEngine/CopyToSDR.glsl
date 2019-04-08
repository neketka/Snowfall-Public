#ifdef VERTEX

layout(location = 0) in vec3 Position;
layout(location = 0) out vec2 Texcoord;

void main()
{
	Texcoord = (Position.xy + 1.0) / 2.0;
	gl_Position = vec4(Position, 1);
}

#endif

#ifdef FRAGMENT

layout(location = 0) out vec4 fragment;
layout(location = 0) in vec2 Texcoord;

layout(location = 0) uniform sampler2D HDR;

void main()
{
	const float oneOverGamma = 1.0 / 2.2;

	vec3 color = pow(texture(HDR, Texcoord).rgb, vec3(oneOverGamma));
	fragment = vec4(color, 1.0);
}

#endif