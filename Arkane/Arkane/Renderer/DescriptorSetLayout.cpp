#include "DescriptorSetLayout.h"

#include "Device.h"

#include "../Core/Assertions.h"
#include "../Core/VulkanAllocator.h"


AK_NAMESPACE_BEGIN


VkShaderStageFlags DescriptorSetLayout::ConvertDescriptorStage(EDescriptorStage _stage)
{
	switch (_stage)
	{
	case Arkane::EDescriptorStage_Vertex:
		return VK_SHADER_STAGE_VERTEX_BIT;
		break;
	case Arkane::EDescriptorStage_TessellationControl:
		return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
		break;
	case Arkane::EDescriptorStage_TessellationEvaluation:
		return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
		break;
	case Arkane::EDescriptorStage_Geometry:
		return VK_SHADER_STAGE_GEOMETRY_BIT;
		break;
	case Arkane::EDescriptorStage_Fragment:
		return VK_SHADER_STAGE_FRAGMENT_BIT;
		break;
	case Arkane::EDescriptorStage_Compute:
		return VK_SHADER_STAGE_COMPUTE_BIT;
		break;
	default:
		akAssertReturnValue(false, VK_SHADER_STAGE_VERTEX_BIT, "Cannot convert from EDescriptorStage to VkShaderStageFlags");
		return VK_SHADER_STAGE_VERTEX_BIT;
		break;
	}
}

VkDescriptorType DescriptorSetLayout::ConvertBindingType(EBindingType _type)
{
	switch (_type)
	{
	case Arkane::EBindingType_Uniform:
		return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		break;
	case Arkane::EBindingType_Sampler:
		return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		break;
	case Arkane::EBindingType_Storage:
		return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		break;
	default:
		akAssertReturnValue(false, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, "Cannot convert from EBindingType to VkDescriptorType");
		return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		break;
	}
}

DescriptorSetLayout::DescriptorSetLayout(SharedPtr<Device> _device) : m_device(_device), m_descriptorSetLayout(VK_NULL_HANDLE)
{

}

DescriptorSetLayout::~DescriptorSetLayout()
{
	if (m_device != VK_NULL_HANDLE && m_descriptorSetLayout != VK_NULL_HANDLE)
	{
		vkDestroyDescriptorSetLayout(m_device->GetDevice(), m_descriptorSetLayout, VulkanAllocator::GetInstance().GetCallbacks());
		m_descriptorSetLayout = VK_NULL_HANDLE;
	}
}

bool DescriptorSetLayout::Create()
{
	akAssertReturnValue(m_bindings.size() > 0, false, "DescriptorSetLayout has no bindings, please Push at least 1 before create it");

	VkDescriptorSetLayoutCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	createInfo.bindingCount = (uint32_t)m_bindings.size();
	createInfo.pBindings = m_bindings.data();

	VkResult result = vkCreateDescriptorSetLayout(m_device->GetDevice(), &createInfo, VulkanAllocator::GetInstance().GetCallbacks(), &m_descriptorSetLayout);

	akAssertReturnValue(result == VK_SUCCESS, false, "Cannot create DescriptorSetLayout");
	return m_descriptorSetLayout != VK_NULL_HANDLE;
}


void DescriptorSetLayout::Push(EDescriptorStage _stage, EBindingType _type, uint32_t _index)
{
	VkDescriptorSetLayoutBinding binding = {};
	binding.descriptorCount = 1;
	binding.stageFlags = ConvertDescriptorStage(_stage);
	binding.descriptorType = ConvertBindingType(_type);
	binding.binding = _index;

	m_bindings.push_back(binding);
}

AK_NAMESPACE_END