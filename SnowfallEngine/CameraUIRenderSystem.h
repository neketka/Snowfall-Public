#pragma once
#include "ECS.h"
#include "export.h"

class CameraUIRenderComponent : public Component
{
public:
	std::string ContextName;
	bool IsSpectator;
};

class CameraUIRenderSystem : public ISystem
{
public:
	SNOWFALLENGINE_API CameraUIRenderSystem();
	SNOWFALLENGINE_API virtual void InitializeSystem(Scene& scene) override;
	SNOWFALLENGINE_API virtual void Update(float deltaTime) override;
	SNOWFALLENGINE_API virtual std::string GetName() override;
	SNOWFALLENGINE_API virtual std::vector<std::string> GetSystemsBefore() override;
	SNOWFALLENGINE_API virtual std::vector<std::string> GetSystemsAfter() override;
	SNOWFALLENGINE_API virtual bool IsMainThread() override { return true; }
private:
	Scene *m_scene;
};
