#pragma once
#include <GL\glew.h>
#include "GLResource.h"
#include <queue>
#include <map>
#include <glm/glm.hpp>

#include "export.h"

class TextureUnitManager
{
public:
	SNOWFALLENGINE_API static void Initialize(GLint availableUnits);
	SNOWFALLENGINE_API static int RefreshUnit(GLint id);
private:
	static std::map<GLint, GLint> m_availableUnits;
	static std::queue<GLint> m_ageSort;
};

enum class WrapMode
{
	Repeat = GL_REPEAT, MirroredRepeat = GL_MIRRORED_REPEAT, ClampToEdge = GL_CLAMP_TO_EDGE,
	ClampToBorder = GL_CLAMP_TO_BORDER
};

enum class TextureChannel
{
	S = GL_TEXTURE_WRAP_S, T = GL_TEXTURE_WRAP_T, R = GL_TEXTURE_WRAP_R
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

class SNOWFALLENGINE_API Sampler : public IGLResource
{
public:
	Sampler();

	void SetWrapMode(TextureChannel channel, WrapMode mode);
	void SetMinificationFilter(MinificationFilter filter);
	void SetMagnificationFilter(MagnificationFilter filter);
	void SetCompareMode(bool enabled);
	void SetComparison(ComparisonFunc func);
	void SetAnisotropicFiltering(bool enabled);
	void SetBorderColor(glm::vec4 color);

	int BindToTextureUnit(); 

	void Destroy() override;

	inline GLuint GetID() { return m_id; }
private:
	GLuint m_id;
};

