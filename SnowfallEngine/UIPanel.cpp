#include "stdafx.h"
#include "UIPanel.h"

void UIPanel::SetBorderEnabled(bool enabled)
{
	m_border = enabled;
}

bool UIPanel::GetBorderEnabled()
{
	return m_border;
}

void UIPanel::OnRenderA(RenderEventArgs& args)
{
	if (m_border)
	{
		args.Renderer->RenderRectangle(GetBounds().RelativeRegion, glm::vec4(0, 0, 0, 1), glm::vec4(0, 0, 0, 1), 2, true);
		Quad2D in = GetBounds().RelativeRegion.Inset(2, 2, 2, 2);
		args.Renderer->PushClip(in);
		RenderChildren(args.Renderer);
		args.Renderer->PopClip();
	}
	else
	{
		args.Renderer->RenderRectangle(GetBounds().RelativeRegion, glm::vec4(0, 0, 0, 1));
		RenderChildren(args.Renderer);
	}
}
