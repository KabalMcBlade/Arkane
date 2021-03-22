#pragma once

#include "../../Core/Defines.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE		// VULKAN
//#define GLM_FORCE_SSE2
#define GLM_FORCE_INTRINSICS
#include "../../Dependencies/glm/glm/glm.hpp"

#include "../../Geometry/Frustum.h"

AK_NAMESPACE_BEGIN

struct Camera
{
	float m_width;
	float m_height;
	float m_zNearP;
	float m_zFarP;
	float m_fov;

	glm::vec3 m_eye;
	glm::vec3 m_at;
	glm::vec3 m_up;
	glm::mat3x3 m_rotationMatrix;
	glm::mat4x4 m_view, m_projection, m_viewProjection;

	Frustum m_frustum;

	glm::mat4x4 m_invView, m_invProjection, m_invViewProjection;
	glm::vec2 m_jitter;
	glm::vec4 m_clipPlane;
};

AK_NAMESPACE_END