#pragma once
#include "UIComponent.h"

class UIViewport : public UIComponent
{
public:
	SNOWFALLENGINE_API UIViewport();

	SNOWFALLENGINE_API void SetViewport(int index);
	SNOWFALLENGINE_API int GetViewport();
private:
	void ChangeSize(BoundsEventArgs& args);
	int m_viewport;
};