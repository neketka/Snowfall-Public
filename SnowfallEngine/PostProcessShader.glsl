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

#ifdef DOWNSAMPLED

layout(location = 0) out vec4 auxillaryFrag;

#else

layout(location = 0) out vec4 fragment;
layout(location = 1) out vec4 auxillaryFrag;

#endif

layout(location = 0) in vec2 Texcoord;

layout(location = 0) uniform float Time;
layout(location = 1) uniform mat4 InvProj;

layout(location = 2) uniform sampler2D Color;
layout(location = 3) uniform sampler2D Auxillary;
layout(location = 4) uniform sampler2D Depth;

float Snowfall_GetTime()
{
	return Time;
}

vec4 Snowfall_GetColor()
{
	return texture(Color, Texcoord);
}

vec4 Snowfall_SampleColor(vec2 texcoord)
{
	return texture(Color, texcoord);
}

vec4 Snowfall_GetAuxillary()
{
	return textureLod(Auxillary, Texcoord, 0);
}

vec4 Snowfall_GetAuxillaryLevel(float level)
{
	return textureLod(Auxillary, Texcoord, level);
}

vec4 Snowfall_SampleAuxillary(vec2 texcoord)
{
	return textureLod(Auxillary, texcoord, 0);
}

vec4 Snowfall_SampleAuxillaryLevel(vec2 texcoord, float level)
{
	return textureLod(Auxillary, texcoord, level);
}

float Snowfall_GetDepth()
{
	return texture(Depth, Texcoord).r;
}

float Snowfall_SampleDepth(vec2 texcoord)
{
	return texture(Depth, texcoord).r;
}

vec3 Snowfall_GetPosition()
{
	return texture(Depth, Texcoord).xyz;
}

#ifndef DOWNSAMPLED

void Snowfall_SetColor(vec4 color)
{
	fragment = color;
}

#endif

void Snowfall_SetAuxillary(vec4 auxillary)
{
	auxillaryFrag = auxillary;
}

vec2 Snowfall_GetTexcoord()
{
	return Texcoord;
}

ivec2 Snowfall_GetResolution()
{
	return textureSize(Color, 0);
}

ivec2 Snowfall_GetResolutionLevel(int level)
{
	return textureSize(Auxillary, level);
}

#ifdef DOWNSAMPLED
#define NO_COLOR ;
#else
#define NO_COLOR Snowfall_SetColor(Snowfall_GetColor())
#endif

#define NO_AUXILLARY Snowfall_SetAuxillary(Snowfall_GetAuxillary())

#endif