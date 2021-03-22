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
// 	glm_vec4 position;
// 	glm_vec4 rotation;
// 	glm_vec4 scale;
 	glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
};

AK_NAMESPACE_END
