#include "stdafx.h"
#include "UIComponent.h"
#include "UIContext.h"

UIComponent::UIComponent() : m_enabled(true), m_visible(true), m_bounds(Quad2D(), glm::vec2(), glm::vec2()), m_focused(false), m_drawChildren(true)
{
}

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
	auto iter = std::upper_bound(m_components.begin(), m_components.end(), component, 
		[](UIComponent *a, UIComponent *b) { return a->GetZIndex() < b->GetZIndex(); });
	component->Parent = this;
	component->m_context = m_context;
	if (!m_enabled)
		component->SetEnabled(false);
	m_components.insert(iter, 1, component);
}

void UIComponent::RemoveComponent(UIComponent *component)
{
	m_components.erase(std::find(m_components.begin(), m_components.end(), component));
	delete component;
}

std::vector<UIComponent*>& UIComponent::GetChildren()
{
	return m_components;
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

	renderer->PushClip(m_bounds.RelativeRegion);

	RenderEventArgs args;
	args.Renderer = renderer;
	OnRender.Fire(args);

	if (m_drawChildren)
		RenderChildren(renderer);

	renderer->PopClip();
}

void UIComponent::RenderChildren(UIRenderer *renderer)
{
	for (UIComponent *comp : m_components)
		comp->Render(renderer);
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
		component->UpdateFocus(lostFocus, gainFocus);
}

void UIComponent::SortByZOrder()
{
	std::sort(m_components.begin(), m_components.end(), [](UIComponent *a, UIComponent *b) { return a->GetZIndex() < b->GetZIndex(); });
}

UIComponent *UIComponent::FindTopmostIntersector(Quad2D quad)
{
	if (!(m_enabled && m_visible))
		return nullptr;
	for (int i = m_components.size() - 1; i >= 0; --i)
	{
		UIComponent *comp = m_components[i]->FindTopmostIntersector(quad);
		if (comp)
			return comp;
	}
	if (m_bounds.RelativeRegion.Intersects(quad))
		return this;
	return nullptr;
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
	if (Parent && !Parent->IsEnabled() && enabled)
		return;
	for (UIComponent *comp : m_components)
		comp->SetEnabled(enabled);
	m_enabled = enabled;
}

bool UIComponent::IsEnabled()
{
	return m_enabled;
}

void UIComponent::SetDrawingChildren(bool drawing)
{
	m_drawChildren = drawing;
}

bool UIComponent::IsDrawingChildren()
{
	return m_drawChildren;
}
