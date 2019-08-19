#pragma once
#include <string>
#include <stack>
#include <glm/glm.hpp>
#include "Quad.h"
#include "TextureAsset.h"
#include "FontAsset.h"
#include "ShaderAsset.h"
#include "MeshAsset.h"
#include "Framebuffer.h"

class UIRenderer
{
public:
	SNOWFALLENGINE_API UIRenderer();
	SNOWFALLENGINE_API ~UIRenderer();

	SNOWFALLENGINE_API void SetupPass(Pipeline& pipe);
	SNOWFALLENGINE_API void StartRenderPass(Framebuffer fbo, int drawBuffer, IQuad2D viewport, glm::mat4 mvp);
	SNOWFALLENGINE_API void StartRenderPass(Framebuffer fbo, int drawBuffer0, int drawBuffer1, IQuad2D viewport, glm::mat4 mvp);
	SNOWFALLENGINE_API void EndRenderPass();

	SNOWFALLENGINE_API void SetBorderedRectangleShader(ShaderAsset *shader);
	SNOWFALLENGINE_API void SetRectangleShader(ShaderAsset *shader);

	SNOWFALLENGINE_API void PushMatrix(glm::mat4 mat);
	SNOWFALLENGINE_API void PopMatrix();

	SNOWFALLENGINE_API void PushClip(IQuad2D quad);
	SNOWFALLENGINE_API void PopClip();

	SNOWFALLENGINE_API void RenderTexture(Quad2D src, Quad2D dest, Texture texture, glm::vec4 tint);
	SNOWFALLENGINE_API void RenderTexture(Quad2D src, Quad2D dest, TextureAsset *texture, glm::vec4 tint);
	SNOWFALLENGINE_API void RenderRectangle(Quad2D quad, glm::vec4 fillColor);
	SNOWFALLENGINE_API void RenderRectangle(Quad2D quad, glm::vec4 borderColor, glm::vec4 fillColor, float borderThickness, bool insetBorder);
	SNOWFALLENGINE_API void RenderRectangle(Quad2D quad, TextureAsset *rectTexture, float borderThickness, glm::vec4 borderTint, glm::vec4 fillTint, bool insetBorder);
	SNOWFALLENGINE_API void RenderText(FontAsset *font, glm::vec2 pos, float size, glm::vec4 color, std::string text);

	inline int GetCurrentDrawBuffer() { return drawTurn ? m_drawBuffer0 : m_drawBuffer1; }
	inline int GetSourceDrawBuffer() { return drawTurn ? m_drawBuffer1 : m_drawBuffer0; }

private:
	Framebuffer m_fbo;
	int m_drawBuffer0;
	int m_drawBuffer1;
	
	bool drawTurn;
	bool twoPass;

	MeshAsset *m_borderQuad;
	MeshAsset *m_basicQuad;

	ShaderAsset *m_rectShader;
	ShaderAsset *m_texShader;

	IQuad2D m_viewport;
	std::stack<IQuad2D> m_clipStack;
	std::stack<glm::mat4> m_matrixStack;

	Sampler m_sampler;
};

