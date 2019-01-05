#pragma once
#include "Camera.h"
#include "Entity.h"

class BasicCamera : public Entity, public ICamera
{
public:
	DEFINE_PROTOTYPE(BasicCamera)
	BasicCamera() : m_region(glm::vec2(0, 0), glm::vec2(800, 600)),
		Entity("BasicCamera", EntityOptions(false, true, false, true, false, false, false, false)) {}

	inline void SetClipping(float zNear, float zFar) { m_zNear = zNear; m_zFar = zFar; }
	inline void SetFOV(float fov) { m_fov = fov; }
	inline void SetAspect(float aspect) { m_aspect = aspect; }
	inline void SetRegion(IQuad2D region) { m_region = region; m_aspect = static_cast<float>(region.Size.x) / static_cast<float>(region.Size.y); }
	inline void SetRenderTarget(Framebuffer fbo) { m_target = fbo; }
	inline void SetUIEnabled(bool enabled) { m_uiEnabled = enabled; }
	inline void SetLayerMask(LayerMask mask) { m_layerMask = mask; }

	inline float GetZNear() { return m_zNear; }
	inline float GetZFar() { return m_zFar; }
	inline float GetFOV() { return m_fov; }
	inline float GetAspect() { return m_aspect; }

	virtual void OnSceneAddition() override {}
	virtual glm::mat4 GetProjectionMatrix() override;
	virtual glm::mat4 GetViewMatrix() override;
	virtual IQuad2D GetRegion() override;
	virtual Framebuffer GetRenderTarget() override;
	virtual bool HasUI() override;
	virtual LayerMask GetLayerMask() override;
private:
	bool m_uiEnabled;
	float m_fov, m_aspect, m_zNear, m_zFar;
	IQuad2D m_region;
	LayerMask m_layerMask = 0xFFFFFFFFFFFFFFFF;
	Framebuffer m_target;
};