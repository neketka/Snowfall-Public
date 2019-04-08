#pragma once
#include "CameraComponent.h"
#include "CommandBuffer.h"
#include "ECS.h"
#include "MeshAsset.h"
#include "export.h"

class SNOWFALLENGINE_API CameraViewportRenderSystem : public ISystem
{
public:
	CameraViewportRenderSystem();
	~CameraViewportRenderSystem();
	virtual void InitializeSystem(Scene & scene) override;
	virtual void Update(float deltaTime) override;
	virtual std::string GetName() override;
	virtual std::vector<std::string> GetSystemsBefore() override;
	virtual std::vector<std::string> GetSystemsAfter() override;
	virtual bool IsMainThread() override;
private:
	Scene *m_scene;
	MeshAsset *m_quad;
	Sampler m_sampler;
	void CopyToSDR(CommandBuffer& buffer, CameraComponent *camera);
};

