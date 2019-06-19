#include "stdafx.h"
#include "UIComponent.h"
#include "UIContext.h"

UIComponent::~UIComponent()
{
	for (UIComponent* comp : m_components)
		delete comp;
}

void UIComponent::Focus()
{
	if (!m_focused)
		m_context->DispatchFocusChange(this);
}

void UIComponent::Unfocus()
{
	if (m_focused)
		m_context->DispatchFocusChange(m_context->GetRootComponent());
}

bool UIComponent::IsFocused()
{
	return m_focused;
}

void UIComponent::AddComponent(UIComponent *component)
{
	m_components.push_back(component);
	component->Parent = this;
	SortByZOrder();
}

void UIComponent::RemoveComponent(UIComponent *component)
{
	m_components.erase(std::find(m_components.begin(), m_components.end(), component));
	delete component;
}

void UIComponent::SetContext(UIContext *context)
{
	m_context = context;
}

UIContext *UIComponent::GetContext()
{
	return m_context;
}

void UIComponent::Render(UIRenderer *renderer)
{
	if (!m_visible)
		return;
	RenderEventArgs args;
	args.Renderer = renderer;
	OnRender.Fire(args);

	renderer->PushClip(IQuad2D(m_bounds.RelativeRegion));

	for (UIComponent *comp : m_components)
		comp->Render(renderer);

	renderer->PopClip();
}

void UIComponent::Resize(Quad2D oldParent, Quad2D newParent)
{
	BoundsEventArgs args;

	args.OldBounds = m_bounds;
	m_bounds.RecalculateRegion(oldParent, newParent);
	args.NewBounds = m_bounds;

	BoundsChanged.Fire(args);
	for (UIComponent *comp : m_components)
		comp->Resize(args.OldBounds.RelativeRegion, m_bounds.RelativeRegion);
}

void UIComponent::UpdateFocus(UIComponent *lostFocus, UIComponent *gainFocus)
{
	FocusEventArgs args;
	m_focused = this == gainFocus;
	args.OldFocused = lostFocus;
	args.NewFocused = gainFocus;
	if (this == gainFocus)
		GainFocus.Fire(args);
	else if (this == lostFocus)
		LostFocus.Fire(args);
	for (UIComponent *component : m_components)
		UpdateFocus(lostFocus, gainFocus);
}

void UIComponent::SortByZOrder()
{
	std::sort(m_components.begin(), m_components.end(), [](UIComponent *a, UIComponent *b) { return a->GetZIndex() < b->GetZIndex(); });
}

UIComponent *UIComponent::FindTopmostIntersector(Quad2D quad)
{
	if (!(m_enabled && m_visible))
		return nullptr;
	if (m_bounds.RelativeRegion.Intersects(quad))
		return this;
	else
	{
		for (int i = m_components.size() - 1; i >= 0; --i)
		{
			UIComponent *comp = m_components[i]->FindTopmostIntersector(quad);
			if (comp)
				return comp;
		}
		return nullptr;
	}
}

UIRect UIComponent::GetBounds()
{
	return m_bounds;
}

void UIComponent::SetBounds(UIRect rect)
{
	BoundsEventArgs args;

	args.OldBounds = m_bounds;
	args.NewBounds = rect;

	m_bounds = rect;
	BoundsChanged.Fire(args);
}

void UIComponent::SetZIndex(int index)
{
	m_zIndex = index;
	if (Parent)
		Parent->SortByZOrder();
}

int UIComponent::GetZIndex()
{
	return m_zIndex;
}

void UIComponent::SetVisible(bool visible)
{
	BoolEventArgs args;
	args.OldValue = m_visible;
	args.NewValue = visible;
	if (!visible)
		Unfocus();
	if (m_visible != visible)
		VisibleChanged.Fire(args);
	m_visible = visible;
}

bool UIComponent::IsVisible()
{
	return m_visible;
}

void UIComponent::SetEnabled(bool enabled)
{
	if (!enabled)
		Unfocus();
	m_enabled = enabled;
}

bool UIComponent::IsEnabled()
{
	return m_enabled;
}
