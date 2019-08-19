#ifndef RENDER_TO_CUBEMAP_INCLUDE
#define RENDER_TO_CUBEMAP_INCLUDE

#ifdef VERTEX

layout(location = 0) out vec4 Direction;

const vec3 directions[24] = vec3[24](
	vec3(-1, -1, 1), vec3(-1, -1, -1), vec3(-1, 1, -1), vec3(-1, 1, 1), // left
	vec3(1, -1, -1), vec3(1, -1, 1), vec3(1, 1, 1), vec3(1, 1, -1), // right
	vec3(-1, -1, -1), vec3(1, -1, -1), vec3(1, -1, 1), vec3(-1, -1, 1), // bottom
	vec3(-1, 1, 1), vec3(1, 1, 1), vec3(1, 1, -1), vec3(-1, 1, -1), // top
	vec3(-1, -1, 1), vec3(1, -1, 1), vec3(1, 1, 1), vec3(-1, 1, 1), // back
	vec3(-1, -1, -1), vec3(1, -1, -1), vec3(1, 1, -1), vec3(-1, 1, -1) // front
);

const vec2 uvs[4] = vec2[4](vec2(-1, -1), vec2(1, -1), vec2(1, 1), vec2(-1, 1));

const int indices[6] = int[6](0, 1, 2, 2, 3, 0);

void main()
{
	int layer = gl_VertexID / 6;
	int index = indices[gl_VertexID - layer * 6];

	Direction = vec4(directions[index + layer * 4], layer);

	gl_Position = vec4(uvs[index], 0, 1);
}

#endif

#ifdef GEOMETRY

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

layout(location = 0) in vec4 in_Direction[];
layout(location = 0) out vec3 out_Direction;

void main()
{
	for (int i = 0; i < 3; ++i)
	{
		vec4 dir = in_Direction[i];
		out_Direction = dir.xyz;

		gl_Layer = int(dir.w);
		gl_Position = gl_in[i].gl_Position;

		EmitVertex();
	}
	EndPrimitive();
}

#endif

#ifdef FRAGMENT

layout(location = 0) out vec4 fragment;

layout(location = 0) in vec3 Direction;

vec3 SampleDirection(vec3 direction);

void main()
{
	fragment = vec4(SampleDirection(Direction), 1);
}

#endif

#endif