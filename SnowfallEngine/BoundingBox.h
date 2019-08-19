#pragma once
#include <glm/glm.hpp>

class BoundingBox
{
public:
	BoundingBox() {}
	BoundingBox(glm::vec3 min, glm::vec3 max);
	~BoundingBox();
	
	glm::vec3 MinExtent;
	glm::vec3 MaxExtent;

	inline glm::vec3 GetCenter() 
	{
		return (MinExtent + MaxExtent) / 2.f;
	}
};

