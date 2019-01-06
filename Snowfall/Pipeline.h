#pragma once
#include <glm\glm.hpp>

#include "Shader.h"
#include "Buffer.h"
#include "VertexArray.h"
#include "Framebuffer.h"
#include "Quad.h"
#include "Sampler.h"

class DrawArraysIndirectCommand
{
public:
	GLuint Count;
	GLuint InstanceCount;
	GLuint First;
	GLuint BaseInstance;
};

class DrawElementsIndirectCommand
{
public:
	GLuint Count;
	GLuint InstanceCount;
	GLuint FirstIndex;
	GLuint BaseVertex;
	GLuint BaseInstance;
};

enum class StencilOperation
{
	Keep = GL_KEEP, Zero = GL_ZERO, Increment = GL_INCR, Decrement = GL_DECR, Invert = GL_INVERT,
	Replace = GL_REPLACE, IncrementWrap = GL_INCR_WRAP, DecrementWrap = GL_DECR_WRAP
};

class ColorMask
{
public:
	int Buffer;
	bool Red;
	bool Green;
	bool Blue;
	bool Alpha;
};

class VertexStage
{
public:
	VertexStage() {}
	
	VertexArray VertexArray;
	
	bool FrontFaceCulling;
	bool BackFaceCulling;

	bool _keepAttributes;
};

class TessellationStage
{
public:
	TessellationStage() {}
	int PatchSize = 1;
};

class FragmentStage
{
public:
	FragmentStage() : Framebuffer(Framebuffer::GetDefault()) {
	}
	Framebuffer Framebuffer;
	std::vector<int> DrawTargets;
	
	IQuad2D Viewport;
	IQuad2D ScissorRegion;
	bool ScissorTest;
	bool RasterizerDiscard;
	bool DepthTest;
	bool StencilTest;

	bool DepthMask;
	GLuint FrontStencilMask;
	GLuint BackStencilMask;
	std::vector<ColorMask> ColorMasks;

	ComparisonFunc FrontStencilFunc;
	ComparisonFunc BackStencilFunc;

	StencilOperation FrontStencilFail;
	StencilOperation FrontDepthFail;
	StencilOperation FrontDepthPass;

	StencilOperation BackStencilFail;
	StencilOperation BackDepthFail;
	StencilOperation BackDepthPass;

	bool Blending;

	bool _keepFBO;
	bool _keepTargets;
};

class TransformFeedbackStage
{
public:
	TransformFeedbackStage() {}
	std::vector<std::pair<int, TBuffer>> BufferBindings;

	bool _keepBindings;
};

class Pipeline
{
public:
	Shader Shader;
	VertexStage VertexStage;
	TessellationStage TessellationStage;
	TransformFeedbackStage TransformFeedbackStage;
	FragmentStage FragmentStage;
	
	void Optimize(Pipeline& previous);

	void BindPipeline();

private:
	bool m_keepShader;
};

