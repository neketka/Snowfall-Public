#include "stdafx.h"

#include "TextRenderer.h"

TextRenderer::TextRenderer(int maxChars)
{
	m_bufferPos = 0;
	m_vao = VertexArray({ BufferStructure({ Attribute(0, 0, 2, 8, true), Attribute(1, 8, 2, 8, true) }) });
	m_data = Buffer<glm::vec4>(maxChars * 6, BufferOptions(false, false, false, false, true, false));
	m_maxChars = maxChars;
	m_width = 0.5f;
	m_edge = 0.3f;

	m_vao.SetBuffer(0, m_data);
	m_vao.SetAttributeEnabled(0, true);
	m_vao.SetAttributeEnabled(1, true);

	m_sample.SetMinificationFilter(MinificationFilter::Linear);
	m_sample.SetMagnificationFilter(MagnificationFilter::Linear);
}

TextRenderer::~TextRenderer()
{
	m_vao.Destroy();
	m_data.Destroy();
	m_sample.Destroy();
}

void TextRenderer::ClearTextBuffer()
{
	m_bufferPos = 0;
}

void TextRenderer::SetColor(glm::vec3 color)
{
	m_color = color;
}

float TextRenderer::GetTextLength(std::string text)
{
	return 0.0f;
}

void TextRenderer::SetDistanceFieldLength(float width, float edge)
{
	m_width = width;
	m_edge = edge;
}

void TextRenderer::SetDistanceFieldLengthAuto(float size, float ppm)
{ // Pretty specific values, not always going to work: relationship between size and edge is strange but it works
	float calcMix = size * ppm / 800.f;
	m_width = glm::mix(0.45f, 0.39f, calcMix);
	m_edge = glm::mix(0.20f, 0.11f, calcMix);
}

void TextRenderer::SetFont(FontAsset& asset)
{
	m_font = &asset;
}

void TextRenderer::RenderTextBuffer(glm::vec2 offset, float scale, std::string text)
{
	if (!m_font)
		return;
	if (text.length() + m_bufferPos / 6 > m_maxChars)
		return;
	const glm::vec2 arr[] = { glm::vec2(0, 0), glm::vec2(1, 0), glm::vec2(1, 1), glm::vec2(1, 1), glm::vec2(0, 1), glm::vec2(0, 0) };
	const glm::vec2 tex[] = { glm::vec2(0, -1), glm::vec2(1, -1), glm::vec2(1, 0), glm::vec2(1, 0), glm::vec2(0, 0), glm::vec2(0, -1) };

	std::vector<glm::vec4> dat;
	float offsetX = 0;
	for (char c : text)
	{
		GlyphDescription desc = m_font->GetGlyph(c);
		for (int i = 0; i < 6; ++i)
		{
			glm::vec2 pos = arr[i] * glm::vec2(desc.CharWidth, desc.CharHeight) + glm::vec2(offsetX, 0);
			glm::vec2 texcoord = tex[i] * glm::vec2(desc.AtlasWidth, desc.AtlasHeight) + glm::vec2(desc.AtlasX, desc.AtlasY);
			dat.push_back(glm::vec4(pos * scale + offset, texcoord));
		}
		offsetX += desc.Advance;
	}
	m_data.CopyData(dat, m_bufferPos);
	m_bufferPos += text.length() * 6;
}

void TextRenderer::RenderText(CommandBuffer& buffer, Framebuffer fbo, int drawBuffer, IQuad2D viewport, glm::mat4 mvp, IQuad2D scissorBox)
{
	if (!m_font)
		return;
	ShaderAsset& shader = AssetManager::LocateAssetGlobal<ShaderAsset>("FontRenderShader");

	BufferBlending blend;

	blend.Buffer = drawBuffer;
	blend.SourceColor = BlendFunction::SourceAlpha;
	blend.SourceAlpha = BlendFunction::SourceAlpha;
	blend.DestinationColor = BlendFunction::OneMinusSourceAlpha;
	blend.DestinationAlpha = BlendFunction::OneMinusSourceAlpha;
	blend.ColorEquation = BlendEquation::Add;
	blend.AlphaEquation = BlendEquation::Add;

	Pipeline pipe;

	pipe.Shader = shader.GetShaderVariant({});
	pipe.VertexStage.VertexArray = m_vao;
	pipe.FragmentStage.Framebuffer = fbo;
	pipe.FragmentStage.DrawTargets = { drawBuffer };
	pipe.FragmentStage.Blending = true;
	pipe.FragmentStage.BufferBlends = { blend };
	pipe.FragmentStage.DepthTest = false;
	pipe.FragmentStage.DepthMask = false;
	pipe.FragmentStage.Viewport = viewport;
	pipe.FragmentStage.ScissorRegion = scissorBox;
	pipe.FragmentStage.ScissorTest = true;
	
	ShaderConstants consts;

	consts.AddConstant(0, mvp);
	consts.AddConstant(1, m_font->GetAtlas(), m_sample);
	consts.AddConstant(2, m_color);
	consts.AddConstant(3, m_width);
	consts.AddConstant(4, m_edge);
	
	buffer.BindPipelineCommand(pipe);
	buffer.BindConstantsCommand(consts);
	buffer.DrawCommand(PrimitiveType::Triangles, 0, m_bufferPos, 1, 0);
}
