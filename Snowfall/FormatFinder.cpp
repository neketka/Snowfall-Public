#include "Texture.h"

template<>
class InternalFormat<32, 32, 32, 32, TextureDataType::Float> { public: const static GLenum format = GL_RGBA32F; };

template<>
class InternalFormat<8, 8, 8, 8, TextureDataType::Int> { public: const static GLenum format = GL_RGBA8I; };

template<>
class PixelFormat<true, true, true, true, false> { public: const static GLenum format = GL_BGRA; };

template<>
class PixelFormat<true, true, true, false, true> { public: const static GLenum format = GL_BGR; };