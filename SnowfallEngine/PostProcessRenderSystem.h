#pragma once
#include "ECS.h"
#include "CameraComponent.h"
#include "MaterialAsset.h"
#include "MeshAsset.h"

class SNOWFALLENGINE_API PostProcessRenderSystem : public ISystem
{
public:
	PostProcessRenderSystem();
	~PostProcessRenderSystem();

	virtual void InitializeSystem(Scene & scene) override;
	virtual void Update(float deltaTime) override;
	virtual std::string GetName() override;
	virtual std::vector<std::string> GetSystemsBefore() override;
	virtual std::vector<std::string> GetSystemsAfter() override;
	virtual bool IsMainThread() override;
private:
	void RunPostprocess(CommandBuffer& buffer, CameraComponent *camera, glm::mat4 invProj, MaterialAsset *postProcess);
	MeshAsset *m_quad;
	Scene *m_scene;
	Sampler m_samplers[3];
};

