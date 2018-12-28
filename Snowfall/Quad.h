#pragma once
#include <glm/glm.hpp>

class Quad2D // Represents a 2D plane space region
{
public:
	Quad2D() { }
	Quad2D(glm::vec2 pos, glm::vec2 size) : Position(pos), Size(size) { }
	glm::vec2 Position;
	glm::vec2 Size;
	bool operator==(const Quad2D& other) { return Position == other.Position && Size == other.Size; }
};