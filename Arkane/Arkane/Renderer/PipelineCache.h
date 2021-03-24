#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Core/Defines.h"
#include "../Core/SmartPointers.h"


#pragma warning( disable : 4251 )	//export std

AK_NAMESPACE_BEGIN

class Device;
class AK_DLL PipelineCache final
{
public:
	PipelineCache(SharedPtr<Device> _device);
	~PipelineCache();

	AK_INLINE const VkPipelineCache& GetPipelineCache() const { return m_pipelineCache; }

private:
	SharedPtr<Device> m_device;
	VkPipelineCache m_pipelineCache;
};

AK_NAMESPACE_END