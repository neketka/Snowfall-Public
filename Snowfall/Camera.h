#pragma once
#include <glm/glm.hpp>
#include "Quad.h"
#include "Framebuffer.h"

using LayerMask = unsigned int;

class ICamera
{
public:
	virtual glm::mat4 GetProjectionMatrix() = 0;
	virtual glm::mat4 GetViewMatrix() = 0;
	virtual IQuad2D GetRegion() = 0;
	virtual Framebuffer GetRenderTarget() = 0;
	virtual LayerMask GetLayerMask() = 0;
	virtual bool HasUI() = 0;
};

