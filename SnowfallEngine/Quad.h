#pragma once
#include <glm/glm.hpp>

class Quad2D // Represents a 2D plane space region
{
public:
	Quad2D() { }
	Quad2D(float x, float y, float w, float h) : Position(x, y), Size(w, h) { }
	Quad2D(glm::vec2 pos, glm::vec2 size) : Position(pos), Size(size) { }
	Quad2D(glm::vec4 quad) : Position(quad.x, quad.y), Size(quad.z, quad.w) { }
	glm::vec2 Position;
	glm::vec2 Size;
	inline Quad2D NoOffset() { return Quad2D(glm::vec2(0, 0), Size); }

	bool operator==(const Quad2D& other) { return Position == other.Position && Size == other.Size; }
	bool operator!=(const Quad2D& other) { return Position != other.Position || Size != other.Size; }
};

class IQuad2D
{
public:
	IQuad2D() { }
	IQuad2D(int x, int y, int w, int h) : Position(x, y), Size(w, h) { }
	IQuad2D(glm::ivec2 pos, glm::ivec2 size) : Position(pos), Size(size) { }
	IQuad2D(glm::ivec4 quad) : Position(quad.x, quad.y), Size(quad.z, quad.w) { }
	glm::ivec2 Position;
	glm::ivec2 Size;
	inline IQuad2D NoOffset() { return IQuad2D(glm::ivec2(0, 0), Size); }

	bool operator==(const IQuad2D& other) { return Position == other.Position && Size == other.Size; }
	bool operator!=(const IQuad2D& other) { return Position != other.Position || Size != other.Size; }
};