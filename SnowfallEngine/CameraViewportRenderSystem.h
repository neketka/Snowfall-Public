#pragma once
#include "CameraComponent.h"
#include "CommandBuffer.h"
#include "ECS.h"
#include "MeshAsset.h"
#include "export.h"

class CameraViewportRenderSystem : public ISystem
{
public:
	SNOWFALLENGINE_API CameraViewportRenderSystem();
	SNOWFALLENGINE_API virtual ~CameraViewportRenderSystem() override;
	SNOWFALLENGINE_API virtual void InitializeSystem(Scene & scene) override;
	SNOWFALLENGINE_API virtual void Update(float deltaTime) override;
	SNOWFALLENGINE_API virtual std::string GetName() override;
	SNOWFALLENGINE_API virtual std::vector<std::string> GetSystemsBefore() override;
	SNOWFALLENGINE_API virtual std::vector<std::string> GetSystemsAfter() override;
	SNOWFALLENGINE_API virtual bool IsMainThread() override;
private:
	Scene *m_scene;
	MeshAsset *m_quad;
	Sampler m_sampler;
	void CopyToSDR(CommandBuffer& buffer, CameraComponent *camera);
};

