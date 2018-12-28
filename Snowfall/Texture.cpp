#include "Texture.h"

std::map<GLint, GLint> ImageUnitManager::m_availableUnits;
std::queue<GLint> ImageUnitManager::m_ageSort;

void ImageUnitManager::Initialize(GLint availableUnits)
{
	for (GLint i = 0; i < availableUnits; ++i)
	{
		m_availableUnits.insert({ i - availableUnits, i });
		m_ageSort.push(i - availableUnits);
	}
}

int ImageUnitManager::RefreshUnit(GLint id)
{
	if (m_availableUnits.find(id) != m_availableUnits.end())
		return m_availableUnits[id];
	else
	{
		GLint oldId = m_ageSort.front();
		m_ageSort.pop();
		GLint unit = m_availableUnits[oldId];
		m_availableUnits.erase(oldId);
		m_availableUnits.insert({ id, unit });
		m_ageSort.push(id);
		return unit;
	}
}

Texture::Texture(int w, int h, bool isCubemap, int levels, GLenum format) : m_format(format), m_w(w), m_h(h)
{
	if (isCubemap)
		glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_id);
	else
		glCreateTextures(GL_TEXTURE_2D, 1, &m_id);
	glTextureStorage2D(m_id, levels, format, w, h);
}

Texture::Texture(int width, int height, int depth, int levels, GLenum format) : m_format(format), m_w(width), m_h(height)
{
	glCreateTextures(GL_TEXTURE_3D, 1, &m_id);
	glTextureStorage3D(m_id, levels, format, width, height, depth);
}

Texture::Texture(int width, int height, int slices, bool isCubemapArray, int levels, GLenum format) : m_format(format), m_w(width), m_h(height)
{
	if (isCubemapArray)
		glCreateTextures(GL_TEXTURE_CUBE_MAP_ARRAY, 1, &m_id);
	else
		glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &m_id);
	glTextureStorage3D(m_id, levels, format, width, height, slices);
}

void Texture::SetData(int x, int y, int w, int h, int level, GLenum format, TextureDataType type, const void *pixels)
{
	glTextureSubImage2D(m_id, level, x, y, w, h, format, static_cast<GLenum>(type), pixels);
}

void Texture::SetData(int x, int y, int z, int w, int h, int d, int level, GLenum format, TextureDataType type, const void * pixels)
{
	glTextureSubImage3D(m_id, level, x, y, z, w, h, d, format, static_cast<GLenum>(type), pixels);
}

void Texture::ReadPixels(int x, int y, int z, int w, int h, int d, int level, GLenum format, TextureDataType type, int bufferSize, void* pixels)
{
	glGetTextureSubImage(m_id, level, x, y, z, w, h, d, format, static_cast<GLenum>(type), bufferSize, pixels);
}

void Texture::Destroy()
{
	glDeleteTextures(1, &m_id);
}

int Texture::m_imagesMade;

Image Texture::CreateImage(GLenum format, int level, bool read, bool write)
{
	return Image(m_imagesMade++, m_id, level, false, 0, read ? write ? GL_READ_WRITE : GL_WRITE_ONLY : GL_READ_ONLY, format);
}

Image Texture::CreateImage(GLenum format, int level, bool read, bool write, int layer)
{
	return Image(m_imagesMade++, m_id, level, true, layer, read ? write ? GL_READ_WRITE : GL_WRITE_ONLY : GL_READ_ONLY, format);
}

WrappedTextureView Texture::CreateWrappedView(int layer, int level)
{
	return WrappedTextureView(layer, level, m_format, m_id);
}

void Texture::GenerateMipmap()
{
	glGenerateTextureMipmap(m_id);
}

int Texture::BindToSampler(Sampler sampler)
{
	int unit = sampler.BindToTextureUnit();
	glBindTextureUnit(unit, m_id);
	return unit;
}
