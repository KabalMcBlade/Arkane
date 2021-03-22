#pragma once


#include "../Core/Defines.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE		// VULKAN
//#define GLM_FORCE_SSE2
#define GLM_FORCE_INTRINSICS
#include "../Dependencies/glm/glm/glm.hpp"


AK_NAMESPACE_BEGIN

struct Frustum
{
	glm::vec4 m_planes[6];
};

AK_NAMESPACE_END