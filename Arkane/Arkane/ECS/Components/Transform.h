#pragma once

#include "../../Core/Defines.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE		// VULKAN
//#define GLM_FORCE_SSE2
#define GLM_FORCE_INTRINSICS
#include "../../Dependencies/glm/glm/glm.hpp"


AK_NAMESPACE_BEGIN

struct Transform
{
// 	glm_vec4 m_position;
// 	glm_vec4 m_rotation;
// 	glm_vec4 m_scale;
 	glm::vec3 m_position;
    glm::vec3 m_rotation;   // radiants - is euler
    glm::vec3 m_scale;
};

AK_NAMESPACE_END
