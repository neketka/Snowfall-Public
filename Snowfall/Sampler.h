#pragma once
#include <GL\glew.h>
#include "GLResource.h"
#include <queue>
#include <map>

class TextureUnitManager
{
public:
	static void Initialize(GLint availableUnits);
	static int RefreshUnit(GLint id);
private:
	static std::map<GLint, GLint> m_availableUnits;
	static std::queue<GLint> m_ageSort;
};

enum class WrapMode
{
	Repeat, MirroredRepeat
};

enum class TextureChannel
{
	S, T, R
};

enum class MinificationFilter
{
	Nearest = GL_NEAREST, Linear = GL_LINEAR, NearestMipmapNearest = GL_NEAREST_MIPMAP_NEAREST, LinearMipmapNearest = GL_LINEAR_MIPMAP_NEAREST,
	NearestMipmapLinear = GL_NEAREST_MIPMAP_LINEAR, LinearMipmapLinear = GL_LINEAR_MIPMAP_LINEAR
};

enum class MagnificationFilter
{
	Nearest = GL_NEAREST, Linear = GL_LINEAR
};

enum class ComparisonFunc
{
	Never = GL_NEVER, Less = GL_LESS, Greater = GL_GREATER, Equal = GL_EQUAL,
	Always = GL_ALWAYS, LessEqual = GL_LEQUAL, GreaterEqual = GL_GEQUAL, NotEqual = GL_NOTEQUAL
};

class Sampler : public IGLResource
{
public:
	Sampler();

	void SetWrapMode(TextureChannel channel, WrapMode mode);
	void SetMinificationFilter(MinificationFilter filter);
	void SetMagnificationFilter(MagnificationFilter filter);
	void SetCompareMode(bool enabled);
	void SetComparison(ComparisonFunc func);

	int BindToTextureUnit(); 

	void Destroy() override;

	inline GLuint GetID() { return m_id; }
private:
	GLuint m_id;
};

