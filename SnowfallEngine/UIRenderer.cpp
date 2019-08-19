#include "stdafx.h"

#include "UIRenderer.h"
#include "TextRenderer.h"

UIRenderer::UIRenderer() : m_viewport(0, 0, 800, 600)
{
	m_matrixStack.push(glm::mat4(1));
	m_clipStack.push(m_viewport);

	m_borderQuad = new MeshAsset("borderQuad", Mesh(
		{ //Border coordinate (x,y), Border Flag (k), Not used (0)
			RenderVertex(glm::vec3(0, 1, 1), glm::vec4(-1, 1, 1, 0), glm::vec2(0, 1)), //0 Top Row
			RenderVertex(glm::vec3(0, 1, 1), glm::vec4(0, 1, 1, 0), glm::vec2(1.f/3.f, 1)),
			RenderVertex(glm::vec3(1, 1, 1), glm::vec4(0, 1, 1, 0), glm::vec2(2.f/3.f, 1)),
			RenderVertex(glm::vec3(1, 1, 1), glm::vec4(1, 1, 1, 0), glm::vec2(1, 1)),

			RenderVertex(glm::vec3(0, 1, 1), glm::vec4(-1, 0, 1, 0), glm::vec2(0, 2.f/3.f)), //4 Top Middle
			RenderVertex(glm::vec3(0, 1, 1), glm::vec4(0, 0, 0, 0), glm::vec2(1.f / 3.f, 2.f / 3.f)),
			RenderVertex(glm::vec3(1, 1, 1), glm::vec4(0, 0, 0, 0), glm::vec2(2.f / 3.f, 2.f / 3.f)),
			RenderVertex(glm::vec3(1, 1, 1), glm::vec4(1, 0, 1, 0), glm::vec2(1, 2.f / 3.f)),

			RenderVertex(glm::vec3(0, 0, 1), glm::vec4(-1, 0, 1, 0), glm::vec2(0, 1.f/3.f)), //8 Bottom Middle
			RenderVertex(glm::vec3(0, 0, 1), glm::vec4(0, 0, 0, 0), glm::vec2(1.f / 3.f, 1.f / 3.f)),
			RenderVertex(glm::vec3(1, 0, 1), glm::vec4(0, 0, 0, 0), glm::vec2(2.f / 3.f, 1.f / 3.f)),
			RenderVertex(glm::vec3(1, 0, 1), glm::vec4(1, 0, 1, 0), glm::vec2(1, 1.f / 3.f)),

			RenderVertex(glm::vec3(0, 0, 1), glm::vec4(-1, -1, 1, 0), glm::vec2(0, 0)), //12 Bottom
			RenderVertex(glm::vec3(0, 0, 1), glm::vec4(0, -1, 1, 0), glm::vec2(1.f / 3.f, 0)),
			RenderVertex(glm::vec3(1, 0, 1), glm::vec4(0, -1, 1, 0), glm::vec2(2.f / 3.f, 0)),
			RenderVertex(glm::vec3(1, 0, 1), glm::vec4(1, -1, 1, 0), glm::vec2(1, 0)),
		},
		{
			4, 1, 0, 4, 5, 1, 5, 2, 1, 5, 6, 2, 6, 3, 2, 6, 7, 3,
			8, 5, 4, 9, 5, 8, 9, 6, 5, 9, 10, 6, 10, 11, 7, 6, 10, 7,
			12, 9, 8, 13, 9, 12, 10, 9, 13, 14, 10, 13, 11, 10, 14, 15, 11, 14
		}
	));

	m_basicQuad = new MeshAsset("basicQuad", Mesh(
		{
			RenderVertex(glm::vec3(0, 0, 1), glm::vec4(0, 0, 1.f/3.f, 1.f/3.f)),
			RenderVertex(glm::vec3(1, 0, 1), glm::vec4(1, 0, 2.f/3.f, 1.f/3.f)),
			RenderVertex(glm::vec3(0, 1, 1), glm::vec4(0, 1, 1.f/3.f, 2.f/3.f)),
			RenderVertex(glm::vec3(1, 1, 1), glm::vec4(1, 1, 2.f/3.f, 2.f/3.f))
		},
		{
			0, 1, 3, 0, 3, 2
		}
	));
}

UIRenderer::~UIRenderer()
{
	m_borderQuad->Unload();
	m_basicQuad->Unload();
	m_sampler.Destroy();

	delete m_borderQuad;
	delete m_basicQuad;
}

void UIRenderer::StartRenderPass(Framebuffer fbo, int drawBuffer, IQuad2D viewport, glm::mat4 mvp)
{
	m_texShader = &AssetManager::LocateAssetGlobal<ShaderAsset>("UIRectangleShader");
	m_rectShader = &AssetManager::LocateAssetGlobal<ShaderAsset>("UIBorderedShader");
	m_viewport = viewport;
	m_drawBuffer0 = drawBuffer;
	m_drawBuffer1 = drawBuffer;
	m_fbo = fbo;
	twoPass = false;

	while (!m_clipStack.empty())
		m_clipStack.pop();

	while (!m_matrixStack.empty())
		m_matrixStack.pop();

	m_clipStack.push(m_viewport);
	m_matrixStack.push(mvp);
}

void UIRenderer::StartRenderPass(Framebuffer fbo, int drawBuffer0, int drawBuffer1, IQuad2D viewport, glm::mat4 mvp)
{
	StartRenderPass(fbo, drawBuffer0, viewport, mvp);
	twoPass = true;
	m_drawBuffer1 = drawBuffer1;
}

void UIRenderer::EndRenderPass()
{
}

void UIRenderer::SetBorderedRectangleShader(ShaderAsset *shader)
{
	m_rectShader = shader;
}

void UIRenderer::SetRectangleShader(ShaderAsset *shader)
{
	m_texShader = shader;
}

void UIRenderer::PushMatrix(glm::mat4 mat)
{
	m_matrixStack.push(m_matrixStack.top() * mat);
}

void UIRenderer::PopMatrix()
{
	if (m_matrixStack.size() > 1)
		m_matrixStack.pop();
}

void UIRenderer::PushClip(IQuad2D quad)
{
	IQuad2D clip = m_clipStack.top();

	quad.Position = glm::clamp(quad.Position, clip.Position, clip.Position + clip.Size);
	quad.Size = glm::min(quad.Size, clip.Size - (quad.Position - clip.Position));

	m_clipStack.push(quad);
}

void UIRenderer::PopClip()
{
	if (m_clipStack.size() > 1)
		m_clipStack.pop();
}

void UIRenderer::RenderTexture(Quad2D src, Quad2D dest, Texture texture, glm::vec4 tint)
{
	CommandBuffer cmd;
	Pipeline p;
	ShaderConstants consts;

	consts.AddConstant(0, m_matrixStack.top());
	consts.AddConstant(1, dest.Position);
	consts.AddConstant(2, dest.Size);
	consts.AddConstant(3, src.Position);
	consts.AddConstant(4, src.Size);
	consts.AddConstant(5, texture, m_sampler);
	consts.AddConstant(6, tint);

	p.Shader = m_texShader->GetShaderVariant({ "TEXTURED" });
	SetupPass(p);

	cmd.BindPipelineCommand(p);
	cmd.BindConstantsCommand(consts);
	m_basicQuad->DrawMeshDirect(cmd);

	cmd.ExecuteCommands();
}

void UIRenderer::RenderTexture(Quad2D src, Quad2D dest, TextureAsset *texture, glm::vec4 tint)
{
	RenderTexture(src, dest, texture->GetTextureObject(), tint);
}

void UIRenderer::RenderRectangle(Quad2D quad, glm::vec4 fillColor)
{
	CommandBuffer cmd;
	Pipeline p;
	ShaderConstants consts;

	consts.AddConstant(0, m_matrixStack.top());
	consts.AddConstant(1, quad.Position);
	consts.AddConstant(2, quad.Size);
	consts.AddConstant(3, fillColor);

	p.Shader = m_texShader->GetShaderVariant({ });
	SetupPass(p);

	cmd.BindPipelineCommand(p);
	cmd.BindConstantsCommand(consts);
	m_basicQuad->DrawMeshDirect(cmd);

	cmd.ExecuteCommands();
}

void UIRenderer::RenderRectangle(Quad2D quad, glm::vec4 borderColor, glm::vec4 fillColor, float borderThickness, bool insetBorder)
{
	if (borderThickness <= 0)
	{
		RenderRectangle(quad, fillColor);
		return;
	}

	if (insetBorder)
	{
		quad.Position += glm::vec2(borderThickness);
		quad.Size -= glm::vec2(borderThickness * 2);
	}

	CommandBuffer cmd;
	Pipeline p;
	ShaderConstants consts;

	consts.AddConstant(0, m_matrixStack.top());
	consts.AddConstant(1, quad.Position);
	consts.AddConstant(2, quad.Size);
	consts.AddConstant(3, borderThickness);
	consts.AddConstant(4, fillColor);
	consts.AddConstant(5, borderColor);

	p.Shader = m_rectShader->GetShaderVariant({ "QUAD" });
	SetupPass(p);

	cmd.BindPipelineCommand(p);
	cmd.BindConstantsCommand(consts);
	m_borderQuad->DrawMeshDirect(cmd);

	cmd.ExecuteCommands();
}

void UIRenderer::RenderRectangle(Quad2D quad, TextureAsset *rectTexture, float borderThickness, glm::vec4 borderTint, glm::vec4 fillTint, bool insetBorder)
{
	if (insetBorder)
	{
		quad.Position += glm::vec2(borderThickness);
		quad.Size -= glm::vec2(borderThickness * 2);
	}

	CommandBuffer cmd;
	Pipeline p;
	ShaderConstants consts;

	consts.AddConstant(0, m_matrixStack.top());
	consts.AddConstant(1, quad.Position);
	consts.AddConstant(2, quad.Size);
	consts.AddConstant(3, borderThickness);
	consts.AddConstant(4, rectTexture->GetTextureObject(), m_sampler);
	consts.AddConstant(5, borderTint);
	consts.AddConstant(6, fillTint);

	p.Shader = m_rectShader->GetShaderVariant({ "TEXTURED" });
	SetupPass(p);

	cmd.BindPipelineCommand(p);
	cmd.BindConstantsCommand(consts);
	m_borderQuad->DrawMeshDirect(cmd);

	cmd.ExecuteCommands();
}

void UIRenderer::RenderText(FontAsset *font, glm::vec2 pos, float size, glm::vec4 color, std::string text)
{
	drawTurn = !drawTurn;

	TextRenderer& renderer = Snowfall::GetGameInstance().GetTextRenderer();
	renderer.ClearTextBuffer();
	renderer.SetColor(color);
	renderer.SetFont(*font);

	glm::mat4 top = m_matrixStack.top();
	renderer.SetDistanceFieldLengthAuto(size, glm::vec3(top[0][0], top[1][1], top[2][2]).length());

	renderer.RenderTextBuffer(pos, size, text);

	CommandBuffer buffer;
	renderer.RenderText(buffer, m_fbo, GetCurrentDrawBuffer(), m_viewport, top, m_clipStack.top());
	buffer.ExecuteCommands();
}

void UIRenderer::SetupPass(Pipeline& p)
{
	drawTurn = !drawTurn;

	BufferBlending blend;

	blend.Buffer = GetCurrentDrawBuffer();
	blend.SourceColor = BlendFunction::SourceAlpha;
	blend.SourceAlpha = BlendFunction::SourceAlpha;
	blend.DestinationColor = BlendFunction::OneMinusSourceAlpha;
	blend.DestinationAlpha = BlendFunction::OneMinusSourceAlpha;
	blend.ColorEquation = BlendEquation::Add;
	blend.AlphaEquation = BlendEquation::Add;

	p.VertexStage.VertexArray = Snowfall::GetGameInstance().GetMeshManager().GetVertexArray();

	p.FragmentStage.Framebuffer = m_fbo;
	p.FragmentStage.DrawTargets = { GetCurrentDrawBuffer() };
	p.FragmentStage.Blending = true;
	p.FragmentStage.BufferBlends = { blend };
	p.FragmentStage.DepthTest = false;
	p.FragmentStage.DepthMask = false;
	p.FragmentStage.Viewport = m_viewport;
	p.FragmentStage.ScissorRegion = m_clipStack.top();
	p.FragmentStage.ScissorTest = true;
}