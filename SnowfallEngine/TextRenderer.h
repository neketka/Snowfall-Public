#pragma once
#include "TextureAsset.h"
#include "CommandBuffer.h"
#include <string>
#include <glm/glm.hpp>

class TextRenderer
{
public:
	TextRenderer();
	~TextRenderer();
	void RenderText(Framebuffer fbo, int drawBuffer, std::string font, std::string text);
};

