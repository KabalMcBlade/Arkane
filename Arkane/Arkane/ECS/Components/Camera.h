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
	enum class CameraType { CameraType_LookAt, CameraType_FirstPerson };


	glm::mat4 m_view;
	glm::mat4 m_proj;

	//float m_width;
	//float m_height;
	float m_zNearPlane;
	float m_zFarPlane;
	float m_fov;

	//glm::vec3 m_eye;
	//glm::vec3 m_at;
	//glm::vec3 m_up;


	CameraType m_type;

	//glm::mat3 m_rotationMatrix;
	//glm::mat4 m_viewProjection;

	//Frustum m_frustum;

	//glm::mat4 m_invView, m_invProjection, m_invViewProjection;
	//glm::vec2 m_jitter;
	//glm::vec4 m_clipPlane;

	float m_rotationSpeed;
	float m_movementSpeed;

	bool m_updated;
};

AK_NAMESPACE_END