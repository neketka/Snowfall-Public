#ifdef VERTEX 

layout(location = 0) in vec3 Position;
layout(location = 1) in vec4 Color;

layout(location = 0) out vec2 TextureUV;
layout(location = 1) out vec2 RectangleUV;

layout(location = 0) uniform mat4 MVP;
layout(location = 1) uniform vec2 DestPos;
layout(location = 2) uniform vec2 DestSize;

#ifdef TEXTURED
layout(location = 3) uniform vec2 SrcPos;
layout(location = 4) uniform vec2 SrcSize;
#else

#endif

void main()
{
#ifdef TEXTURED
	TextureUV = mix(SrcPos, SrcPos + SrcSize, Color.xy);
#else
	TextureUV = Color.xy;
#endif

	RectangleUV = Color.xy;
	gl_Position = MVP * vec4(Position.xy * DestSize + DestPos, 1, 1);
}

#endif

#ifdef FRAGMENT

layout(location = 0) out vec4 Fragment;

layout(location = 0) in vec2 TextureUV;
layout(location = 1) in vec2 RectangleUV;

#ifdef TEXTURED
layout(location = 5) uniform sampler2D Texture;
layout(location = 6) uniform vec4 Tint;
#else
layout(location = 3) uniform vec4 FillColor;
#endif

vec2 GetTextureUV()
{
	return TextureUV;
}

vec2 GetRectangleUV()
{
	return RectangleUV;
}

vec4 SampleColor(vec2 uv)
{
#ifdef TEXTURED
	return texture(Texture, uv);
#else
	return FillColor;
#endif
}

vec4 GetCurrentColor()
{
	return SampleColor(GetTextureUV());
}

vec4 GetTint()
{
#ifdef TEXTURED
	return Tint;
#else
	return vec4(1);
#endif
}

vec4 FragmentColor();

void main()
{
	Fragment = FragmentColor();
}

#ifndef CUSTOM_FRAGMENT
vec4 FragmentColor()
{
	return GetCurrentColor() * GetTint();
}
#endif

#endif