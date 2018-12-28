#include "Framebuffer.h"

Framebuffer::Framebuffer()
{
	glCreateFramebuffers(1, &m_id);
}

Framebuffer::Framebuffer(std::vector<WrappedTextureView> colorAttachments)
{
	glCreateFramebuffers(1, &m_id);
	int index = 0;
	for (WrappedTextureView colorAttachment : colorAttachments)
	{
		glNamedFramebufferTextureLayer(m_id, GL_COLOR_ATTACHMENT0 + index, colorAttachment.GetID(),
			colorAttachment.GetLevel(), colorAttachment.GetLayer());
		++index;
	}
}

Framebuffer::Framebuffer(WrappedTextureView depth, bool hasStencil)
{
	glCreateFramebuffers(1, &m_id);
	glNamedFramebufferTextureLayer(m_id, hasStencil ? GL_DEPTH_STENCIL_ATTACHMENT : GL_DEPTH_ATTACHMENT, depth.GetID(), depth.GetLevel(),
		depth.GetLayer());
}

Framebuffer::Framebuffer(std::vector<WrappedTextureView> colorAttachments, WrappedTextureView depth, bool hasStencil)
{
	glCreateFramebuffers(1, &m_id);
	int index = 0;
	for (WrappedTextureView colorAttachment : colorAttachments)
	{
		glNamedFramebufferTextureLayer(m_id, GL_COLOR_ATTACHMENT0 + index, colorAttachment.GetID(), 
			colorAttachment.GetLevel(), colorAttachment.GetLayer());
		++index;
	}
	glNamedFramebufferTextureLayer(m_id, hasStencil ? GL_DEPTH_STENCIL_ATTACHMENT : GL_DEPTH_ATTACHMENT, depth.GetID(), depth.GetLevel(),
		depth.GetLayer());
}

void Framebuffer::ClearColor(int drawBuffer, glm::vec4 color)
{
	if (m_id == 0)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT);
	}
	else
		glClearNamedFramebufferfv(m_id, GL_COLOR, drawBuffer, &color[0]);
}

void Framebuffer::ClearDepth(float value)
{
	glClearNamedFramebufferfv(m_id, GL_DEPTH, 0, &value);
}

void Framebuffer::Destroy()
{
	glDeleteFramebuffers(1, &m_id);
}
