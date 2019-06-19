#ifdef VERTEX 

layout(location = 0) in vec3 Position;
layout(location = 1) in vec4 Color;
layout(location = 4) in vec2 Texcoord;

#ifdef TEXTURED
layout(location = 0) out vec2 TextureUV;
#endif

layout(location = 1) flat out float BorderFlag;

layout(location = 0) uniform mat4 MVP;
layout(location = 1) uniform vec2 DestPos;
layout(location = 2) uniform vec2 DestSize;
layout(location = 3) uniform float Border;

void main()
{
	gl_Position = MVP * vec4(Position.xy * DestSize + DestPos + Border * Color.xy, 1, 1);
	BorderFlag = Color.z;
#ifdef TEXTURED
	TextureUV = Texcoord;
#endif
}

#endif

#ifdef FRAGMENT

layout(location = 0) out vec4 Fragment;

#ifdef TEXTURED
layout(location = 0) in vec2 TextureUV;
#endif
layout(location = 1) flat in float BorderFlag;

#ifdef TEXTURED
layout(location = 4) uniform sampler2D Texture;
layout(location = 5) uniform vec4 BorderAlphaTint;
layout(location = 6) uniform vec4 FillAlphaTint;
#else
layout(location = 4) uniform vec4 FillColor;
layout(location = 5) uniform vec4 BorderColor;
#endif

void main()
{
#ifdef TEXTURED
	Fragment = texture(Texture, TextureUV) * mix(FillAlphaTint, BorderAlphaTint, BorderFlag);
#else
	Fragment = mix(FillColor, BorderColor, BorderFlag);
#endif
}

#endif