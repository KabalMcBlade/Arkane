#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
#include <stdint.h>


//////////////////////////////////////////////////////////////////////////
// DEPENDENCIES
//#define VMEM_OVERRIDE_NEW_DELETE
//#include "Dependencies/VMem/VMem.h"
//#include "Dependencies/VMem/VMem.cpp"
#include "Dependencies/VulkanMemoryAllocator/src/VmaUsage.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE		// VULKAN
#define GLM_FORCE_SIMD_AVX2
#include "Dependencies/glm/glm/glm.hpp"


//////////////////////////////////////////////////////////////////////////
// CORE
#include "Core/Defines.h"
#include "Core/Assertions.h"
#include "Core/VulkanAllocator.h"


//////////////////////////////////////////////////////////////////////////
// CLIENT
#include "Client/CommandLineParser.h"
#include "Client/FileSystem.h"


//////////////////////////////////////////////////////////////////////////
// RENDERER
#include "Renderer/QueueFamily.h"
#include "Renderer/Instance.h"
#include "Renderer/Device.h"


//////////////////////////////////////////////////////////////////////////
// ECS
#include "ECS/Types.h"


//////////////////////////////////////////////////////////////////////////
// CLIENT
#include "Client/CommandLineParser.h"
#include "Client/FileSystem.h"
#include "Client/EngineApp.h"
