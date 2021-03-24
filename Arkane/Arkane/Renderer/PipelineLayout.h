#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Core/Defines.h"
#include "../Core/SmartPointers.h"

#include <vector>


#pragma warning( disable : 4251 )	//export std

AK_NAMESPACE_BEGIN

class Device;
class DescriptorSetLayout;
class AK_DLL PipelineLayout final
{
public:
	PipelineLayout(SharedPtr<Device> _device, SharedPtr<DescriptorSetLayout> _descriptorSetLayout);
	~PipelineLayout();

	bool Create();

	AK_INLINE operator const VkPipelineLayout& () const
	{
		return m_pipelineLayout;
	}

	AK_INLINE const VkPipelineLayout& GetLayout() const
	{
		return m_pipelineLayout;
	}

	AK_INLINE const size_t GetPushConstantSize() const
	{
		return m_pushContant.size();
	}

	AK_INLINE const VkPushConstantRange GetPushConstant(uint32_t _index) const
	{
		return m_pushContant[_index];
	}

	void PushConstant(VkShaderStageFlags _stage, uint32_t _offset, uint32_t _size);

private:
	SharedPtr<Device> m_device;
	SharedPtr<DescriptorSetLayout> m_descriptorSetLayout;
	VkPipelineLayout m_pipelineLayout;
	std::vector<VkPushConstantRange> m_pushContant;
};

AK_NAMESPACE_END
