#include "Sampler.h"

std::map<GLint, GLint> TextureUnitManager::m_availableUnits;
std::queue<GLint> TextureUnitManager::m_ageSort;

void TextureUnitManager::Initialize(GLint availableUnits)
{
	for (GLint i = 0; i < availableUnits; ++i)
	{
		m_availableUnits.insert({ i - availableUnits, i });
		m_ageSort.push(i - availableUnits);
	}
}

int TextureUnitManager::RefreshUnit(GLint id)
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

Sampler::Sampler()
{
	glCreateSamplers(1, &m_id);
}

void Sampler::SetWrapMode(GLint mode)
{
	glSamplerParameteri(m_id, GL_TEXTURE_WRAP_S, mode);
	glSamplerParameteri(m_id, GL_TEXTURE_WRAP_T, mode);
	glSamplerParameteri(m_id, GL_TEXTURE_WRAP_R, mode);
}

void Sampler::SetMinificationFilter(GLint filter)
{
	glSamplerParameteri(m_id, GL_TEXTURE_MIN_FILTER, filter);
}

void Sampler::SetMagnificationFilter(GLint filter)
{
	glSamplerParameteri(m_id, GL_TEXTURE_MAG_FILTER, filter);
}

int Sampler::BindToTextureUnit()
{
	int unit = TextureUnitManager::RefreshUnit(m_id);
	glBindSampler(unit, m_id);
	return unit;
}

void Sampler::Destroy()
{
	glDeleteSamplers(1, &m_id);
}
