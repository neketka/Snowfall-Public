#include "Pipeline.h"

void Pipeline::Optimize(Pipeline& previous)
{ 
	// Minimize state changes
	if (Shader.GetID() == previous.Shader.GetID())
		m_keepShader = true;

	if (this->VertexStage.VertexArray.GetID() == previous.VertexStage.VertexArray.GetID())
		this->VertexStage._keepAttributes = true;
	
	if (this->TransformFeedbackStage.BufferBindings == previous.TransformFeedbackStage.BufferBindings)
		this->TransformFeedbackStage._keepBindings = true;

	if (this->FragmentStage.Framebuffer.GetID() == previous.FragmentStage.Framebuffer.GetID())
		this->FragmentStage._keepFBO = true;

	if (this->FragmentStage.DrawTargets == previous.FragmentStage.DrawTargets)
		this->FragmentStage._keepTargets = true;
}

void Pipeline::BindPipeline()
{
	if (!this->m_keepShader)
		glUseProgram(this->Shader.GetID());
	//Vertex Stage
	if (!this->VertexStage._keepAttributes)
		glBindVertexArray(this->VertexStage.VertexArray.GetID());

	//Tesselation Stage
	glPatchParameteri(GL_PATCH_VERTICES, this->TessellationStage.PatchSize);

	//Transform Feedback Stage
	if (!this->TransformFeedbackStage._keepBindings)
		for (std::pair<int, TBuffer> buffers : this->TransformFeedbackStage.BufferBindings)
			glBindBufferBase(GL_TRANSFORM_FEEDBACK, buffers.first, buffers.second.GetID());

	//Fragment Stage
	if (!this->FragmentStage._keepFBO)
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->FragmentStage.Framebuffer.GetID());

	if (this->FragmentStage.DepthTest) glEnable(GL_DEPTH_TEST);
	else glDisable(GL_DEPTH_TEST);

	if (this->FragmentStage.RasterizerDiscard) glEnable(GL_RASTERIZER_DISCARD);
	else glDisable(GL_RASTERIZER_DISCARD);
	
	glViewport(static_cast<GLint>(this->FragmentStage.Viewport.Position.x), static_cast<GLint>(this->FragmentStage.Viewport.Position.y),
			static_cast<GLsizei>(this->FragmentStage.Viewport.Size.x), static_cast<GLsizei>(this->FragmentStage.Viewport.Size.y));

	if (!this->FragmentStage._keepTargets && this->FragmentStage.Framebuffer.GetID() != 0)
	{
		std::vector<GLenum> drawTargets;
		for (int target : this->FragmentStage.DrawTargets)
			drawTargets.push_back(GL_COLOR_ATTACHMENT0 + target);
		glDrawBuffers(static_cast<GLsizei>(this->FragmentStage.DrawTargets.size()), drawTargets.data());
	}
}
