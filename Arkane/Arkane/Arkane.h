#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
#include <stdint.h>


//////////////////////////////////////////////////////////////////////////
// DEPENDENCIES
#include "Dependencies/VulkanMemoryAllocator/src/VmaUsage.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE		// VULKAN
//#define GLM_FORCE_SS2
#define GLM_FORCE_INTRINSICS
#include "Dependencies/glm/glm/glm.hpp"


//////////////////////////////////////////////////////////////////////////
// CORE
#include "Core/Defines.h"
#include "Core/Assertions.h"
#include "Core/VulkanAllocator.h"
#include "Core/SmartPointers.h"


//////////////////////////////////////////////////////////////////////////
// Utilities
#include "Utilities/Hash.h"


//////////////////////////////////////////////////////////////////////////
// Event
#include "Events/Event.h"
#include "Events/EventManager.h"


//////////////////////////////////////////////////////////////////////////
// CLIENT
#include "Client/CommandLineParser.h"
#include "Client/FileSystem.h"


//////////////////////////////////////////////////////////////////////////
// RENDERER
#include "Renderer/QueueFamily.h"
#include "Renderer/Instance.h"
#include "Renderer/Device.h"
#include "Renderer/SwapChain.h"


//////////////////////////////////////////////////////////////////////////
// ECS
#include "ECS/Core/Types.h"
#include "ECS/Core/ComponentArray.h"
#include "ECS/Systems/System.h"
#include "ECS/Managers/SystemManager.h"
#include "ECS/Managers/EntityManager.h"
#include "ECS/Managers/ComponentManager.h"
#include "ECS/Managers/ECSManager.h"


//////////////////////////////////////////////////////////////////////////
// GEOMETRY
#include "Geometry/Frustum.h"


//////////////////////////////////////////////////////////////////////////
// COMPONENTS
#include "ECS/Components/Transform.h"
#include "ECS/Components/Renderable.h"
#include "ECS/Components/Camera.h"


//////////////////////////////////////////////////////////////////////////
// CLIENT
#include "Client/CommandLineParser.h"
#include "Client/FileSystem.h"
#include "Client/EngineApp.h"
