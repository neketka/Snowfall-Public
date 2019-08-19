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
layout(location = 0) uniform sampler2D img;

void main()
{
	float brightness = dot(texture(img, Texcoord).xyz, vec3(0.2126, 0.7152, 0.0722)) * smoothstep(0, 1, length(Texcoord - vec2(0.5, 0.5)) * 2);
	fragment = vec4(log(brightness));
}

#endif