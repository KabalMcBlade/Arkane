#pragma once

#include "../../Core/Defines.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE		// VULKAN
//#define GLM_FORCE_SSE2
#define GLM_FORCE_INTRINSICS
#include "../../Dependencies/glm/glm/glm.hpp"

#include "../../Renderer/VertexCache.h"


AK_NAMESPACE_BEGIN

struct Renderable
{
	glm::mat4 m_model;
};

AK_NAMESPACE_END