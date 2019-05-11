#ifdef VERTEX

layout(location = 0) in vec2 Position;
layout(location = 1) in vec2 Texcoord;

layout(location = 0) uniform mat4 MVP;
layout(location = 0) out vec2 out_Texcoord;

void main()
{
	out_Texcoord = Texcoord;
	gl_Position = MVP * vec4(Position, 1, 1);
}

#endif

#ifdef FRAGMENT

layout(location = 0) out vec4 fragment;

layout(location = 0) in vec2 in_Texcoord;

layout(location = 1) uniform sampler2D FontAtlas;
layout(location = 2) uniform vec3 Color;
layout(location = 3) uniform float Width;
layout(location = 4) uniform float Edge;

void main()
{
	float distance = 1.0 - texture(FontAtlas, in_Texcoord).r;
	float alpha = 1.0 - smoothstep(Width, Width + Edge, distance);

	fragment = vec4(Color, alpha);
}

#endif