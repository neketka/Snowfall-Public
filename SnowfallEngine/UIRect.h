#pragma once

#include <glm/glm.hpp>
#include "Quad.h"

class UIRect 
{
public:
	UIRect(Quad2D region, glm::vec2 blAnchor, glm::vec2 trAnchor) : RelativeRegion(region),
		AnchorTopRight(trAnchor), AnchorBottomLeft(blAnchor){}

	UIRect() {}

	glm::vec2 AnchorTopRight;
	glm::vec2 AnchorBottomLeft;

	Quad2D RelativeRegion;

	void RecalculateRegion(Quad2D oldParent, Quad2D parent)
	{
		glm::vec2 topRight = RelativeRegion.Position + RelativeRegion.Size;
		glm::vec2 bottomLeft = RelativeRegion.Position;

		glm::vec2 kFactor = glm::vec2(parent.Size.x / oldParent.Size.x, parent.Size.y / oldParent.Size.y);

		glm::vec2 oldDistBL = bottomLeft - AnchorBottomLeft;
		glm::vec2 oldDistTR = topRight - AnchorTopRight;

		AnchorTopRight = AnchorTopRight * kFactor;
		AnchorBottomLeft = AnchorBottomLeft * kFactor;

		bottomLeft = AnchorBottomLeft + oldDistBL;
		topRight = AnchorTopRight + oldDistTR;

		RelativeRegion = Quad2D(glm::min(topRight, bottomLeft), glm::abs(topRight - bottomLeft));
	}
};