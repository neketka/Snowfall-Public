#ifndef SNOWFALL_SHADER_INCLUDE
#define SNOWFALL_SHADER_INCLUDE

#define PI 3.14159265359

#include "Lighting"
#include "DeferredLighting"

#ifdef VERTEX

layout(location = 0) in vec3 Position;
layout(location = 1) in vec4 Color;
layout(location = 2) in vec3 Normal;
layout(location = 3) in vec3 Tangent;
layout(location = 4) in vec2 Texcoord;

#ifndef SHADOWPASS

layout(location = 0) out vec3 out_Position;
layout(location = 1) out vec4 out_Color;
layout(location = 2) out vec3 out_Normal;
layout(location = 3) out vec3 out_Tangent;
layout(location = 4) out vec2 out_Texcoord;
layout(location = 5) out int out_ObjectId;
layout(location = 6) out int out_ParamCount;

#endif

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
	vec3 Position;
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
#ifndef SHADOWPASS
	gl_Position = Snowfall_WorldToClipSpace(oData.Position);
	out_Position = oData.Position;
	out_Color = oData.Color;
	out_Normal = oData.Normal;
	out_Tangent = oData.Tangent;
	out_Texcoord = oData.Texcoord;
	out_ObjectId = Snowfall_GetObjectID();
	out_ParamCount = ParamCount;
#else
	gl_Position = vec4(oData.Position, 1.0);
#endif
}

#ifndef CUSTOM_VERTEX

void main()
{
	VertexOutputData data;

	data.Position = Snowfall_GetWorldSpacePosition();
	data.Color = Snowfall_GetColor();
	data.Normal = Snowfall_GetWorldSpaceNormal();
	data.Tangent = Snowfall_GetTangent();
	data.Texcoord = Snowfall_GetTexcoord();

	Snowfall_SetOutputData(data);
}

#endif

#endif

#ifdef GEOMETRY

#ifdef CUBEPASS
#define MAX_VERTICES 18
#define LAYER (Layer * 6 + f)
#define MATRIX_INDEX f
#else
#define MAX_VERTICES 3
#define LAYER (Layer)
#define MATRIX_INDEX 0
#endif

layout(triangles) in;
layout(triangle_strip, max_vertices = MAX_VERTICES) out;

#ifndef SHADOWPASS
layout(location = 0) in vec3 in_Position[];
layout(location = 1) in vec4 in_Color[];
layout(location = 2) in vec3 in_Normal[];
layout(location = 3) in vec3 in_Tangent[];
layout(location = 4) in vec2 in_Texcoord[];
layout(location = 5) in int in_ObjectId[];
layout(location = 6) in int in_ParamCount[];

layout(location = 0) out vec3 out_Position;
layout(location = 1) out vec4 out_Color;
layout(location = 2) out vec3 out_Normal;
layout(location = 3) out vec3 out_Tangent;
layout(location = 4) out vec2 out_Texcoord;
layout(location = 5) out int out_ObjectId;
layout(location = 6) out int out_ParamCount;
#endif

#ifdef SHADOWPASS

layout(location = 5) uniform mat4 LightMatrices[6];
layout(location = 11) uniform int Layer;

#endif

void main()
{
#ifdef CUBEPASS
	for (int f = 0; f < 6; ++f)
	{
#endif
		for (int i = 0; i < 3; ++i)
		{
#ifndef SHADOWPASS
			out_Position = in_Position[i];
			out_Color = in_Color[i];
			out_Normal = in_Normal[i];
			out_Tangent = in_Tangent[i];
			out_Texcoord = in_Texcoord[i];
			out_ObjectId = in_ObjectId[i];
			out_ParamCount = in_ParamCount[i];
#endif
#ifdef SHADOWPASS
			gl_Layer = LAYER;
			gl_Position = LightMatrices[MATRIX_INDEX] * gl_in[i].gl_Position;
#else
			gl_Position = gl_in[i].gl_Position;
#endif
			EmitVertex();
		}
		EndPrimitive();
#ifdef CUBEPASS
	}
#endif
}

#endif

#ifdef SHADOWPASS
#ifdef FRAGMENT

#undef FRAGMENT

void main()
{
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
layout(location = 4) uniform vec3 CamPos;

layout(location = 12) uniform sampler2DShadow HighDirectionalShadow;
layout(location = 13) uniform sampler2DArrayShadow FlatShadows;
layout(location = 14) uniform samplerCubeArrayShadow CubeShadows;

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

vec3 Snowfall_GetCameraPosition()
{
	return CamPos;
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

#ifndef CUSTOM_FRAGMENT

layout(location = 0) out vec4 fragment;

void Snowfall_SetMaterialData(Material mat)
{
	vec3 color = vec3(0.0, 0.0, 0.0);

	const int DIRECTIONAL_SHADOW_SAMPLES = 16;
	const int SHADOW_SAMPLES = 9;

	const float DIRECTIONAL_SHADOW_SPAN = sqrt(DIRECTIONAL_SHADOW_SAMPLES) / 2.0;
	const float SHADOW_SPAN = sqrt(SHADOW_SAMPLES) / 2.0;

	for (int i = 0; i < PassLightCount; ++i)
	{
		Light light = AllLights[PassLightIndices[i]];
		float shadow = 1.0;

		if (allInvocations(light.HighIndex == -1))
		{
			shadow = 1.0;
		}
		else
		{
			ivec2 offset = ivec2(2, 2);
			vec4 points = vec4(1.0, 1.0, 1.0, 1.0);
			vec4 shadowCoord = light.LightSpace * vec4(Snowfall_GetPosition(), 1);

			vec2 flatTexels = 1.0 / textureSize(FlatShadows, 0).xy;
			vec2 cubeTexels = 1.0 / textureSize(CubeShadows, 0).xy;
			vec2 dirTexels = 1.0 / textureSize(HighDirectionalShadow, 0);

			shadow = 0.0;

			switch (light.Type)
			{
			case 0:
				for (float y = -DIRECTIONAL_SHADOW_SPAN; y < DIRECTIONAL_SHADOW_SPAN; y += 1.0)
					for (float x = -DIRECTIONAL_SHADOW_SPAN; x < DIRECTIONAL_SHADOW_SPAN; x += 1.0)
						shadow += texture(HighDirectionalShadow, vec3(shadowCoord.xy + vec2(x, y) * dirTexels, shadowCoord.z));
				shadow /= DIRECTIONAL_SHADOW_SAMPLES;
				break;
			case 1:
				//shadow = texture(CubeShadows, );
				break;
			case 2:
				shadowCoord /= shadowCoord.w;
				for (float y = -SHADOW_SPAN; y < SHADOW_SPAN; y += 1.0)
					for (float x = -SHADOW_SPAN; x < SHADOW_SPAN; x += 1.0)
						shadow += texture(FlatShadows, vec4(shadowCoord.xy + vec2(x, y) * flatTexels * shadowCoord.w, light.HighIndex, shadowCoord.z));
				shadow /= SHADOW_SAMPLES;
				break;
			}
		}
		color += CalculateLight(Snowfall_GetCameraPosition(), Snowfall_GetPosition(), light, mat, shadow);
	}
	fragment = vec4(color + mat.Emissive, 1.0);
}

#endif

#endif
#endif