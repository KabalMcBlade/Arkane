#include "DescriptorSet.h"

#include "Device.h"
#include "DescriptorPool.h"
#include "DescriptorSetLayout.h"

#include "../Core/Assertions.h"


AK_NAMESPACE_BEGIN

DescriptorSet::DescriptorSet(SharedPtr<Device> _device) : m_device(_device), m_descriptorSet(VK_NULL_HANDLE)
{
}

DescriptorSet::~DescriptorSet()
{
// 	if (m_device != VK_NULL_HANDLE)
// 	{
// 	
// 	}
}

bool DescriptorSet::Create(SharedPtr<DescriptorPool> _descriptorPool)
{
	akAssertReturnValue(m_layouts.size() > 0, false, "DescriptorSet has no layout, please Push at least 1 before create it");
	akAssertReturnValue(m_descriptors.size() > 0, false, "DescriptorSet has no descriptor, please Push at least 1 before create it");

	VkDescriptorSetAllocateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	createInfo.descriptorPool = _descriptorPool->GetDescritorPool();
	createInfo.pSetLayouts = m_layouts.data();
	createInfo.descriptorSetCount = static_cast<uint32_t>(m_layouts.size());

	VkResult result = vkAllocateDescriptorSets(m_device->GetDevice(), &createInfo, &m_descriptorSet);
	akAssertReturnValue(result == VK_SUCCESS, false, "Cannot create DescriptorSet");


	for (auto& descriptor : m_descriptors)
	{
		descriptor.dstSet = m_descriptorSet;
	}

	vkUpdateDescriptorSets(m_device->GetDevice(), static_cast<uint32_t>(m_descriptors.size()), m_descriptors.data(), 0, nullptr);

	return true;
}

void DescriptorSet::PushLayout(SharedPtr<DescriptorSetLayout> _layout)
{
	m_layouts.push_back(_layout->GetDescriptorLayout());
}

void DescriptorSet::PushDescriptor(VkWriteDescriptorSet _descriptor)
{
	m_descriptors.push_back(_descriptor);
}

void DescriptorSet::PushDescriptor(uint32_t _binding, VkDescriptorType _type, const VkDescriptorBufferInfo* _bufferInfo, uint32_t _descriptorCount /*= 1*/)
{
	VkWriteDescriptorSet writeDescriptorSet{};
	writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeDescriptorSet.descriptorType = _type;
	writeDescriptorSet.dstBinding = _binding;
	writeDescriptorSet.pBufferInfo = _bufferInfo;
	writeDescriptorSet.descriptorCount = _descriptorCount;
	m_descriptors.push_back(writeDescriptorSet);
}

void DescriptorSet::PushDescriptor(uint32_t _binding, VkDescriptorType _type, const VkDescriptorImageInfo* _imageInfo, uint32_t _descriptorCount /*= 1*/)
{
	VkWriteDescriptorSet writeDescriptorSet{};
	writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeDescriptorSet.descriptorType = _type;
	writeDescriptorSet.dstBinding = _binding;
	writeDescriptorSet.pImageInfo = _imageInfo;
	writeDescriptorSet.descriptorCount = _descriptorCount;
	m_descriptors.push_back(writeDescriptorSet);
}

AK_NAMESPACE_END