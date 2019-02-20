#ifndef SNOWFALL_SHADER_INCLUDE
#define SNOWFALL_SHADER_INCLUDE

#define PI 3.14159265359

#ifdef VERTEX

layout(location = 0) in vec3 Position;
layout(location = 1) in vec4 Color;
layout(location = 2) in vec3 Normal;
layout(location = 3) in vec3 Tangent;
layout(location = 4) in vec2 Texcoord;

layout(location = 0) out vec3 out_Position;
layout(location = 1) out vec4 out_Color;
layout(location = 2) out vec3 out_Normal;
layout(location = 3) out vec3 out_Tangent;
layout(location = 4) out vec2 out_Texcoord;
layout(location = 5) out int out_ObjectId;
layout(location = 6) out int out_ParamCount;

layout(location = 0) uniform mat4 ProjectionMatrix;
layout(location = 1) uniform mat4 ViewMatrix;
layout(location = 2) uniform int ParamCount;
layout(location = 3) uniform int ObjectIdOffset;

layout(std430, binding = 0) buffer DynamicTransformBuffer
{
	mat4 TransformMatrices[];
};

layout(std430, binding = 1) buffer ObjectParamsBuffer
{
	vec4 ObjectParameters[];
};

vec4 Snowfall_GetObjectParameter(int index)
{
#ifdef INSTANCED
	return ObjectParameters[(gl_InstanceID + ObjectIdOffset) * ParamCount];
#else
	return ObjectParameters[(gl_DrawID + ObjectIdOffset) * ParamCount];
#endif
}

struct VertexOutputData
{
	vec4 Position;
	vec4 Color;
	vec3 Normal;
	vec3 Tangent;
	vec2 Texcoord;
};

int Snowfall_GetObjectID()
{
#ifdef INSTANCED
	return gl_InstanceID;
#else
	return gl_DrawID + ObjectIdOffset;
#endif
}

vec3 Snowfall_GetWorldSpacePosition()
{
#ifdef STATIC
	return Position;
#else
	return vec3(TransformMatrices[Snowfall_GetObjectID() * 2] * vec4(Position, 1));
#endif
}

vec4 Snowfall_GetColor()
{
	return Color;
}

vec3 Snowfall_GetWorldSpaceNormal()
{
#ifdef STATIC
	return Normal;
#else
	return mat3(TransformMatrices[Snowfall_GetObjectID() * 2 + 1]) * Normal;
#endif
}

vec3 Snowfall_GetTangent()
{
#ifdef STATIC
	return Tangent;
#else
	return mat3(TransformMatrices[Snowfall_GetObjectID() * 2 + 1]) * Tangent;
#endif
}

vec2 Snowfall_GetTexcoord()
{
	return Texcoord;
}

vec4 Snowfall_WorldToClipSpace(vec3 worldSpace)
{
	return ProjectionMatrix * ViewMatrix * vec4(worldSpace, 1);
}

void Snowfall_SetOutputData(VertexOutputData oData)
{
	gl_Position = oData.Position;

	out_Position = Snowfall_GetWorldSpacePosition();
	out_Color = oData.Color;
	out_Normal = oData.Normal;
	out_Tangent = oData.Tangent;
	out_Texcoord = oData.Texcoord;
	out_ObjectId = Snowfall_GetObjectID();
	out_ParamCount = ParamCount;
}

#ifndef CUSTOM_VERTEX

void main()
{
	VertexOutputData data;

	data.Position = Snowfall_WorldToClipSpace(Snowfall_GetWorldSpacePosition());
	data.Color = Snowfall_GetColor();
	data.Normal = Snowfall_GetWorldSpaceNormal();
	data.Tangent = Snowfall_GetTangent();
	data.Texcoord = Snowfall_GetTexcoord();

	Snowfall_SetOutputData(data);
}

#endif

#endif

#ifdef FRAGMENT

layout(location = 0) in vec3 Position;
layout(location = 1) in vec4 Color;
#ifdef FLAT_NORMAL
layout(location = 2) flat in vec3 Normal;
#else
layout(location = 2) in vec3 Normal;
#endif
layout(location = 3) in vec3 Tangent;
layout(location = 4) in vec2 Texcoord;
layout(location = 5) flat in int ObjectId;
layout(location = 6) flat in int ParamCount;

layout(std430, binding = 1) buffer ObjectParamsBuffer
{
	vec4 ObjectParameters[];
};

vec4 Snowfall_GetObjectParameter(int index)
{
	return ObjectParameters[ObjectId * ParamCount];
}

vec3 Snowfall_GetPosition()
{
	return Position;
}

vec4 Snowfall_GetColor()
{
	return Color;
}

vec3 Snowfall_GetNormal()
{
	return Normal;
}

vec3 Snowfall_GetTangent()
{
	return Tangent;
}

vec3 Snowfall_GetBinormal()
{
	return cross(Normal, Tangent);
}

vec2 Snowfall_GetTexcoord()
{
	return Texcoord;
}

#endif
#endif