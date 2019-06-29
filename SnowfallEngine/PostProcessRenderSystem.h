#pragma once
#include "ECS.h"
#include "CameraComponent.h"
#include "MaterialAsset.h"
#include "MeshAsset.h"

class PostProcessRenderSystem : public ISystem
{
public:
	SNOWFALLENGINE_API PostProcessRenderSystem();
	SNOWFALLENGINE_API virtual ~PostProcessRenderSystem() override;

	SNOWFALLENGINE_API virtual void InitializeSystem(Scene & scene) override;
	SNOWFALLENGINE_API virtual void Update(float deltaTime) override;
	SNOWFALLENGINE_API virtual std::string GetName() override;
	SNOWFALLENGINE_API virtual std::vector<std::string> GetSystemsBefore() override;
	SNOWFALLENGINE_API virtual std::vector<std::string> GetSystemsAfter() override;
	SNOWFALLENGINE_API virtual bool IsMainThread() override;
private:
	void RunPostprocess(CommandBuffer& buffer, CameraComponent *camera, glm::mat4 invProj, MaterialAsset *postProcess);
	MeshAsset *m_quad;
	Scene *m_scene;
	Sampler m_samplers[4];
};

