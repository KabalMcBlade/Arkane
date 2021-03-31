#include "DescriptorPool.h"

#include "Device.h"

#include "../Core/Assertions.h"
#include "../Core/VulkanAllocator.h"


AK_NAMESPACE_BEGIN

DescriptorPool::DescriptorPool(SharedPtr<Device> _device) : m_device(_device), m_descriptorPool(VK_NULL_HANDLE)
{

}

DescriptorPool::~DescriptorPool()
{
	if (m_device != VK_NULL_HANDLE && m_descriptorPool != VK_NULL_HANDLE)
	{
		vkDestroyDescriptorPool(m_device->GetDevice(), m_descriptorPool, VulkanAllocator::GetInstance().GetCallbacks());
		m_descriptorPool = VK_NULL_HANDLE;
	}
}

bool DescriptorPool::Create(VkDescriptorPoolCreateFlags _flag /*= VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT*/, uint32_t _maxDescriptorSet /*= 16384*/)
{
	akAssertReturnValue(m_poolSizes.size() > 0, false, "DescriptorPool is Empty, please Push at least 1 before create it");

	VkDescriptorPoolCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	createInfo.poolSizeCount = static_cast<uint32_t>(m_poolSizes.size());
	createInfo.pPoolSizes = m_poolSizes.data();
	createInfo.maxSets = _maxDescriptorSet;
	createInfo.flags = _flag;

	VkResult result = vkCreateDescriptorPool(m_device->GetDevice(), &createInfo, VulkanAllocator::GetInstance().GetCallbacks(), &m_descriptorPool);
	akAssertReturnValue(result == VK_SUCCESS, false, "Cannot create DescriptorPool");

	return m_descriptorPool != VK_NULL_HANDLE;
}

void DescriptorPool::Push(VkDescriptorType _type, uint32_t _descriptorCount)
{
	VkDescriptorPoolSize descriptorPoolSize{};
	descriptorPoolSize.type = _type;
	descriptorPoolSize.descriptorCount = _descriptorCount;
	m_poolSizes.push_back(descriptorPoolSize);
}

AK_NAMESPACE_END