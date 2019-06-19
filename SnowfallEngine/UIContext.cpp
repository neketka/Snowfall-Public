#include "stdafx.h"
#include "UIContext.h"

UIContext::~UIContext()
{
}

void UIContext::UpdateContext(UIRenderer & uiRenderer, float time)
{
}

void UIContext::SetRootComponent(UIComponent * component)
{
}

void UIContext::SetBounds(IQuad2D bounds)
{
}

void UIContext::SetDoubleClickTime(float time)
{
}

void UIContext::SetHoverTime(float time)
{
}

void UIContext::SetInputManager(InputManager * manager)
{
}

void UIContext::SetDispatchingEvents(bool dispatching)
{
}

void UIContext::SetDispatchingTimers(bool dispatching)
{
}

void UIContext::DispatchFocusChange(UIComponent * component)
{
}

void UIContext::SetTimer(TimedEvent event)
{
}

IQuad2D UIContext::GetBounds()
{
	return IQuad2D();
}

float UIContext::GetDoubleClickTime()
{
	return 0.0f;
}

float UIContext::GetHoverTime()
{
	return 0.0f;
}

bool UIContext::IsDispatchingEvents()
{
	return false;
}

bool UIContext::IsDispatchingTimers()
{
	return false;
}

UIComponent * UIContext::GetRootComponent()
{
	return nullptr;
}
