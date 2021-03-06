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
#include "Dependencies/glm/glm/matrix.hpp"
#include "Dependencies/glm/glm/ext/matrix_transform.hpp"
#include "Dependencies/glm/glm/ext/matrix_clip_space.hpp"


//////////////////////////////////////////////////////////////////////////
// CORE
#include "Core/Defines.h"
#include "Core/Assertions.h"
#include "Core/VulkanAllocator.h"
#include "Core/SmartPointers.h"
#include "Core/MemCpy.h"


//////////////////////////////////////////////////////////////////////////
// Utilities
#include "Utilities/Hash.h"


//////////////////////////////////////////////////////////////////////////
// Event
#include "Events/CallbackHandle.h"
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
#include "Renderer/VertexLayout.h"
#include "Renderer/VertexDescriptor.h"
#include "Renderer/RenderPass.h"
#include "Renderer/DescriptorSetLayout.h"
#include "Renderer/PipelineLayout.h"
#include "Renderer/PipelineCache.h"
#include "Renderer/Pipeline.h"
#include "Renderer/FrameBuffer.h"
#include "Renderer/CommandPool.h"
#include "Renderer/StagingManager.h"
#include "Renderer/Buffers/BaseBufferObject.h"
#include "Renderer/Buffers/VertexBufferObject.h"
#include "Renderer/Buffers/IndexBufferObject.h"
#include "Renderer/Buffers/UniformBufferObject.h"
#include "Renderer/CommandBuffer.h"
#include "Renderer/Frame.h"
#include "Renderer/DescriptorPool.h"
#include "Renderer/DescriptorSet.h"
#include "Renderer/VertexCache.h"
#include "Renderer/RenderManager.h"


//////////////////////////////////////////////////////////////////////////
// ECS CORE
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
// SHADERS
#include "Shaders/Shader.h"
#include "Shaders/ShaderManager.h"


//////////////////////////////////////////////////////////////////////////
// COMPONENTS
#include "ECS/Components/Transform.h"
#include "ECS/Components/Renderable.h"
#include "ECS/Components/Camera.h"


//////////////////////////////////////////////////////////////////////////
// SYSTEMS
#include "ECS/Systems/CameraSystem.h"
#include "ECS/Systems/RenderSystem.h"


//////////////////////////////////////////////////////////////////////////
// CLIENT
#include "Client/CommandLineParser.h"
#include "Client/FileSystem.h"
#include "Client/EngineApp.h"
