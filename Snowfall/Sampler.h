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

class Sampler : public IGLResource
{
public:
	Sampler();

	void SetWrapMode(GLint mode);
	void SetMinificationFilter(GLint filter);
	void SetMagnificationFilter(GLint filter);

	int BindToTextureUnit(); 

	void Destroy() override;

	inline GLuint GetID() { return m_id; }
private:
	GLuint m_id;
};

