#pragma once
#include "TextureAsset.h"
#include "CommandBuffer.h"
#include "FontAsset.h"
#include "CameraComponent.h"
#include "TransformComponent.h"
#include <string>
#include <glm/glm.hpp>

class TextRenderer
{
public:
	TextRenderer(int maxChars);
	~TextRenderer();
	void ClearTextBuffer();
	void SetColor(glm::vec3 color);
	float GetTextLength(std::string text);
	void SetDistanceFieldLength(float width, float edge);
	void SetDistanceFieldLengthAuto(float size, float ppm);
	void SetFont(FontAsset& asset);
	void RenderTextBuffer(glm::vec2 offset, float scale, std::string text);
	void RenderText(CommandBuffer& buffer, Framebuffer fbo, int drawBuffer, IQuad2D viewport, glm::mat4 mvp, IQuad2D scissorBox);
private:
	float m_width, m_edge;
	glm::vec3 m_color;
	Sampler m_sample;
	FontAsset *m_font;
	int m_bufferPos;
	int m_maxChars;
	Buffer<glm::vec4> m_data;
	VertexArray m_vao;
};

