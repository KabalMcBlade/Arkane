#include "PipelineLayout.h"

#include "DescriptorSetLayout.h"
#include "Device.h"

#include "../Core/Assertions.h"
#include "../Core/VulkanAllocator.h"


AK_NAMESPACE_BEGIN


PipelineLayout::PipelineLayout(SharedPtr<Device> _device, SharedPtr<DescriptorSetLayout> _descriptorSetLayout) : m_device(_device), m_descriptorSetLayout(_descriptorSetLayout), m_pipelineLayout(VK_NULL_HANDLE)
{
}

PipelineLayout::~PipelineLayout()
{
	if (m_device != VK_NULL_HANDLE && m_pipelineLayout != VK_NULL_HANDLE)
	{
		vkDestroyPipelineLayout(m_device->GetDevice(), m_pipelineLayout, VulkanAllocator::Instance().GetCallbacks());
		m_pipelineLayout = VK_NULL_HANDLE;
	}
}

bool PipelineLayout::Create()
{
	VkPipelineLayoutCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	createInfo.setLayoutCount = 1;
	createInfo.pSetLayouts = m_descriptorSetLayout->GetDescriptorLayoutPtr();

	if (m_pushContant.size() > 0)
	{
		createInfo.pushConstantRangeCount = static_cast<uint32_t>(m_pushContant.size());
		createInfo.pPushConstantRanges = m_pushContant.data();
	}

	VkResult result = vkCreatePipelineLayout(m_device->GetDevice(), &createInfo, VulkanAllocator::Instance().GetCallbacks(), &m_pipelineLayout);

	akAssertReturnValue(result == VK_SUCCESS, false, "Cannot create PipelineLayout");
	return m_pipelineLayout != VK_NULL_HANDLE;
}


void PipelineLayout::PushConstant(VkShaderStageFlags _stage, uint32_t _offset, uint32_t _size)
{
	VkPushConstantRange pushConstantRange{};
	pushConstantRange.stageFlags = _stage;
	pushConstantRange.offset = _offset;
	pushConstantRange.size = _size;

	m_pushContant.push_back(pushConstantRange);
}

AK_NAMESPACE_END