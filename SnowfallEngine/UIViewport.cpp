#include "stdafx.h"
#include "UIViewport.h"

UIViewport::UIViewport()
{
	m_viewport = 1;
	Snowfall::GetGameInstance().CreateViewport(1);
	BoundsChanged.Subscribe(&UIViewport::ChangeSize, this);
}

void UIViewport::SetViewport(int index)
{
	m_viewport = index;
}

int UIViewport::GetViewport()
{
	return m_viewport;
}

void UIViewport::ChangeSize(BoundsEventArgs& args)
{
	Snowfall::GetGameInstance().SetViewportCoefficients(m_viewport, Quad2D(), args.NewBounds.RelativeRegion);
}
