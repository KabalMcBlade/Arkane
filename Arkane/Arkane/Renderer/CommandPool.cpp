#include "CommandPool.h"

#include "../Core/Assertions.h"
#include "../Core/VulkanAllocator.h"

#include "Device.h"

AK_NAMESPACE_BEGIN

CommandPool::CommandPool(SharedPtr<Device> _device, uint32_t _queueFamilyIndex, VkCommandPoolCreateFlags _flags /*= 0*/) : m_device(_device), m_commandPool(VK_NULL_HANDLE)
{
	m_device = _device;
	VkCommandPoolCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	createInfo.queueFamilyIndex = _queueFamilyIndex;
	createInfo.flags = _flags;

	VkResult result = vkCreateCommandPool(m_device->GetDevice(), &createInfo, VulkanAllocator::Instance().GetCallbacks(), &m_commandPool);
	akAssertReturnVoid(result == VK_SUCCESS, "Cannot create CommandPool");
}

CommandPool::~CommandPool()
{
	if (m_device != VK_NULL_HANDLE && m_commandPool != VK_NULL_HANDLE)
	{
		vkDestroyCommandPool(m_device->GetDevice(), m_commandPool, VulkanAllocator::Instance().GetCallbacks());
		m_commandPool = VK_NULL_HANDLE;
	}
}

AK_NAMESPACE_END
