#pragma once
#include <glm/glm.hpp>
#include <vector>

#include "export.h"

class RenderVertex
{
public:
	RenderVertex() {}

	RenderVertex(glm::vec3 pos)
		: Position(pos) {}

	RenderVertex(glm::vec3 pos, glm::vec4 color)
		: Position(pos), Color(color) {}

	RenderVertex(glm::vec3 pos, glm::vec4 color, glm::vec2 texcoord)
		: Position(pos), Color(color), Texcoord(texcoord) {}

	RenderVertex(glm::vec3 pos, glm::vec4 color, glm::vec3 normal, glm::vec2 texcoord)
		: Position(pos), Color(color), Normal(normal), Texcoord(texcoord) {}

	RenderVertex(glm::vec3 pos, glm::vec4 color, glm::vec3 normal, glm::vec3 tangent, glm::vec2 texcoord) 
		: Position(pos), Color(color), Normal(normal), Tangent(tangent), Texcoord(texcoord) {}

	glm::vec3 Position;
	glm::vec4 Color;
	glm::vec3 Normal;
	glm::vec3 Tangent;
	glm::vec2 Texcoord;
};

class Mesh
{
public:
	Mesh() {}
	Mesh(std::vector<RenderVertex> vertices, std::vector<int> indices)
		: Vertices(vertices), Indices(indices) {}

	SNOWFALLENGINE_API void CalculateNormals();
	SNOWFALLENGINE_API void CalculateTangents();

	std::vector<RenderVertex> Vertices;

	std::vector<int> Indices;
};

