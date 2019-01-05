#ifndef SNOWFALL_SHADER_INCLUDE
#define SNOWFALL_SHADER_INCLUDE

#define PI 3.14159265359

#ifdef VERTEX

layout(location = 0) in vec3 Position;
layout(location = 1) in vec4 Color;
layout(location = 2) in vec3 Normal;
layout(location = 3) in vec3 Tangent;
layout(location = 4) in vec2 Texcoord;
layout(location = 5) in int ObjectId;

layout(location = 0) out vec3 out_Position;
layout(location = 1) out vec4 out_Color;
layout(location = 2) out vec3 out_Normal;
layout(location = 3) out vec3 out_Tangent;
layout(location = 4) out vec2 out_Texcoord;
layout(location = 5) out int out_ObjectId;

layout(location = 0) uniform mat4 ProjectionMatrix;
layout(location = 1) uniform mat4 ViewMatrix;

#ifdef DYNAMIC
layout(std140, binding = 0) uniform DynamicTransformBuffer
{
	mat4 ModelMatrices[MAX_DYNAMIC_OBJECTS];
	mat4 NormalMatrices[MAX_DYNAMIC_OBJECTS];
};

#ifdef HAS_PARAMS
layout(std140, binding = 1) uniform ObjectParamsBuffer
{
	vec4 ObjectParameters[OBJECT_PARAMS * MAX_DYNAMIC_OBJECTS];
};

vec4 Snowfall_GetObjectParameter(int index)
{
	return ObjectParameters[ObjectId * OBJECT_PARAMS];
}
#endif

#endif

struct VertexOutputData
{
	vec4 Position;
	vec4 Color;
	vec3 Normal;
	vec3 Tangent;
	vec2 Texcoord;
};

void Snowfall_SetOutputData(VertexOutputData oData)
{
	gl_Position = oData.Position;

	out_Position = oData.Position.xyz;
	out_Color = oData.Color;
	out_Normal = oData.Normal;
	out_Tangent = oData.Tangent;
	out_Texcoord = oData.Texcoord; 
	out_ObjectId = ObjectId;
}

vec3 Snowfall_GetWorldSpacePosition()
{
#ifdef STATIC
	return Position;
#else
	return vec3(ModelMatrices[ObjectId] * vec4(Position, 1));
#endif
}

vec4 Snowfall_GetColor()
{
	return Color;
}

int Snowfall_GetObjectID()
{
	return ObjectId;
}

vec3 Snowfall_GetWorldSpaceNormal()
{
#ifdef STATIC
	return Normal;
#else
	return mat3(NormalMatrices[ObjectId]) * Normal;
#endif
}

vec3 Snowfall_GetTangent()
{
#ifdef STATIC
	return Tangent;
#else
	return mat3(NormalMatrices[ObjectId]) * Tangent;
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

#ifdef HAS_PARAMS
layout(std140, binding = 1) uniform ObjectParamsBuffer
{
	vec4 ObjectParameters[OBJECT_PARAMS * MAX_DYNAMIC_OBJECTS];
};

vec4 Snowfall_GetObjectParameter(int index)
{
	return ObjectParameters[ObjectId * OBJECT_PARAMS];
}
#endif

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

int Snowfall_GetObjectID()
{
	return ObjectId;
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