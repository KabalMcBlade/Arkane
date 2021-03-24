#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Core/Defines.h"
#include "../Core/SmartPointers.h"


#pragma warning( disable : 4251 )	//export std

AK_NAMESPACE_BEGIN

class Device;
class AK_DLL CommandPool
{
public:
	CommandPool(SharedPtr<Device> _device, uint32_t _queueFamilyIndex, VkCommandPoolCreateFlags _flags = 0);
	~CommandPool();

	AK_INLINE const VkCommandPool& GetCommandPool() const
	{
		return m_commandPool;
	}

private:
	SharedPtr<Device> m_device;
	VkCommandPool m_commandPool;
};

AK_NAMESPACE_END