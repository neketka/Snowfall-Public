#pragma once
#include <glm\glm.hpp>

#include "Texture.h"

class ShaderConstants
{
public:
	void AddConstant(int location, Texture texture, Sampler sampler);
	void AddConstant(int location, Image image);
	void AddConstant(int location, int i);
	void AddConstant(int location, float f);
	void AddConstant(int location, glm::vec2 v2);
	void AddConstant(int location, glm::vec3 v3);
	void AddConstant(int location, glm::vec4 v4);
	void AddConstant(int location, glm::mat3 m3);
	void AddConstant(int location, glm::mat4 m4);
	void AddConstantRange(int location, std::vector<float> data);
	void AddConstantRange(int location, std::vector<glm::vec4> data);

	void BindConstants();
private:
	std::vector<std::pair<int, std::pair<Texture, Sampler>>> m_uniformT;
	std::vector<std::pair<int, Image>> m_uniformI;
	std::vector<std::pair<int, int>> m_uniform1i;
	std::vector<std::pair<int, float>> m_uniform1f;
	std::vector<std::pair<int, glm::vec2>> m_uniform2f;
	std::vector<std::pair<int, glm::vec3>> m_uniform3f;
	std::vector<std::pair<int, glm::vec4>> m_uniform4f;
	std::vector<std::pair<int, glm::mat3>> m_uniform3fvMat;
	std::vector<std::pair<int, glm::mat4>> m_uniform4fvMat;
	std::vector<std::pair<int, std::vector<float>>> m_uniform1fv;
	std::vector<std::pair<int, std::vector<glm::vec4>>> m_uniform4fv;
};

