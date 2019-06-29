#include "stdafx.h"

#include "BoundingBox.h"

BoundingBox::BoundingBox(glm::vec3 min, glm::vec3 max)
{
	MinExtent = min;
	MaxExtent = max;
}

BoundingBox::~BoundingBox()
{
}
