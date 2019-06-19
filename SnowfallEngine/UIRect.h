#pragma once

#include <glm/glm.hpp>
#include "Quad.h"

class UIRect 
{
public:
	glm::vec2 AnchorTopRight;
	glm::vec2 AnchorBottomLeft;

	Quad2D RelativeRegion;

	void RecalculateRegion(Quad2D oldParent, Quad2D parent)
	{
		glm::vec2 topRight = RelativeRegion.Position + RelativeRegion.Size;
		glm::vec2 bottomLeft = RelativeRegion.Position;

		glm::vec2 anchorTRPixel = AnchorTopRight * oldParent.Size;
		glm::vec2 anchorBLPixel = AnchorBottomLeft * oldParent.Size;

		glm::vec2 kFactor = glm::vec2(oldParent.Size.x / parent.Size.x, oldParent.Size.y / parent.Size.y);

		topRight += anchorTRPixel - AnchorTopRight;
		bottomLeft += anchorBLPixel - AnchorBottomLeft;

		RelativeRegion = Quad2D(glm::min(topRight, bottomLeft), glm::abs(topRight - bottomLeft));
	}
};