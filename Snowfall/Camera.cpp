#include "Camera.h"
#include "BasicCamera.h"
#include <glm/gtx/transform.hpp>
#include <fstream>

glm::mat4 BasicCamera::GetProjectionMatrix()
{
	return glm::perspective(m_fov, m_aspect, m_zNear, m_zFar);
}

glm::mat4 BasicCamera::GetViewMatrix()
{
	auto mat = glm::rotate(Transformable::Rotation.y, glm::vec3(0, 1, 0)) *
		glm::rotate(Transformable::Rotation.x, glm::vec3(1, 0, 0)) *
		glm::rotate(Transformable::Rotation.z, glm::vec3(0, 0, 1)) *
		glm::translate(Transformable::Position);
	return mat;
}

Quad2D BasicCamera::GetRegion()
{
	return m_region;
}

Framebuffer BasicCamera::GetRenderTarget()
{
	return m_target;
}

bool BasicCamera::HasUI()
{
	return m_uiEnabled;
}

