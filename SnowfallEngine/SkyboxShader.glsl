#ifdef VERTEX

layout(location = 0) in vec3 Position;
layout(location = 0) out vec3 out_Direction;
layout(location = 0) uniform mat4 Inv;

void main()
{
	out_Direction = (Inv * vec4(Position, 1)).xyz;
	gl_Position = vec4(Position, 1);
}

#endif

#ifdef FRAGMENT

layout(location = 0) out vec4 fragment;

layout(location = 0) in vec3 Direction;
layout(location = 1) uniform samplerCube Skybox;

void main()
{
	fragment = texture(Skybox, Direction * vec3(1, -1, 1));
}

#endif