#pragma once
#include "TextureAsset.h"
#include "RenderTargetAsset.h"
#include "ECS.h"

#include "export.h"

class SNOWFALLENGINE_API ShadowMapRenderSystem : public ISystem
{
public:
	ShadowMapRenderSystem();
	virtual ~ShadowMapRenderSystem() override;

	virtual void InitializeSystem(Scene& scene) override;
	virtual void Update(float deltaTime) override;
	virtual std::string GetName() override;
	virtual std::vector<std::string> GetSystemsBefore() override;
	virtual std::vector<std::string> GetSystemsAfter() override;
	virtual bool IsMainThread() override;

	inline TextureAsset *GetDirectionHighShadowTexture() { return m_directionalHighShadow; }
	inline TextureAsset *GetCubeShadowTexture() { return m_cubeShadows; }
	inline TextureAsset *GetFlatShadowTexture() { return m_flatShadows; }
private:
	void DoCloseDirectionalPass(glm::mat4 matrix, LayerMask mask, int size);
	void DoPointPass(std::vector<glm::mat4> matrices, std::vector<LayerMask> masks, int size);
	void DoSpotPass(std::vector<glm::mat4> matrices, std::vector<LayerMask> masks, int size);

	TextureAsset *m_directionalHighShadow;
	TextureAsset *m_cubeShadows;
	TextureAsset *m_flatShadows;

	RenderTargetAsset *m_highShadowTarget;
	RenderTargetAsset *m_cubeShadowTarget;
	RenderTargetAsset *m_flatShadowTarget;

	int m_cubeShadowsCount;
	int m_flatShadowsCount;

	Scene *m_scene;
};

