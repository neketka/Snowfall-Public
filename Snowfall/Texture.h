#pragma once
#include <GL\glew.h>
#include <map>
#include <queue>

#include "GLResource.h"
#include "Sampler.h"

enum class TextureDataType {
	Float=GL_FLOAT, Int=GL_INT, Unsigned=GL_UNSIGNED_BYTE
};

template<int r, int g, int b, int a, TextureDataType>
class InternalFormat { public: const static GLenum format; };

template<bool r, bool g, bool b, bool a, bool reverse>
class PixelFormat { public: const static GLenum format; };

class ImageUnitManager
{
public:
	static void Initialize(GLint availableUnits);
	static int RefreshUnit(GLint id);
private:
	static std::map<GLint, GLint> m_availableUnits;
	static std::queue<GLint> m_ageSort;
};

class Image
{
public:
	Image(GLint iCreated, GLuint tex, GLint level, bool layered, GLint layer, GLenum access, GLenum format) :
		m_iCreated(iCreated), m_tex(tex), m_level(level), m_layered(layered), m_layer(layer), m_access(access), m_format(format) {}
	int BindToUnit() 
	{
		auto unit = ImageUnitManager::RefreshUnit(m_iCreated);
		glBindImageTexture(unit, m_tex, m_level, m_layered, m_layer, m_access, m_format);
		return unit;
	}
private:
	GLuint m_tex; 
	GLint m_level;
	bool m_layered; 
	GLint m_layer;
	GLenum m_access; 
	GLenum m_format;
	GLint m_iCreated;
};

class WrappedTextureView
{
public:
	WrappedTextureView(int layer, int level, GLuint format, GLuint id)
		: m_layer(layer), m_level(level), m_format(format), m_id(id) {}

	inline int GetLayer() { return m_layer; }
	inline int GetLevel() { return m_level; }
	inline GLuint GetFormat() { return m_format; }
	inline GLuint GetID() { return m_id; }
private:
	int m_layer;
	int m_level;
	GLuint m_format;
	GLuint m_id;
};

class Texture : public IGLResource
{
public:
	Texture(int width, int height, bool isCubemap, int levels, GLenum format);
	Texture(int width, int height, int depth, int levels, GLenum format);
	Texture(int width, int height, int slices, bool isCubemapArray, int levels, GLenum format);
	void GenerateMipmap();
	void SetData(int x, int y, int w, int h, int level, GLenum format, TextureDataType type, const void *pixels);
	void SetData(int x, int y, int z, int w, int h, int d, int level, GLenum format, TextureDataType type, const void *pixels);
	void ReadPixels(int x, int y, int z, int w, int h, int d, int level, GLenum format, TextureDataType type, int bufferSize, void *pixels);
	int BindToSampler(Sampler sampler);
	Image CreateImage(GLenum format, int level, bool read, bool write);
	Image CreateImage(GLenum format, int level, bool read, bool write, int layer);

	WrappedTextureView CreateWrappedView(int layer, int level);

	void Destroy() override;

	inline GLuint GetID() { return m_id; }
	inline GLuint GetFormat() { return m_format; }
	inline int GetWidth() { return m_w; }
	inline int GetHeight() { return m_h; }
private:
	int m_w, m_h;
	static int m_imagesMade;
	GLuint m_id;
	GLuint m_format;
};