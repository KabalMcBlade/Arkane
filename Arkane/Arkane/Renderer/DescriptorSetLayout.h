#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Core/Defines.h"
#include "../Core/SmartPointers.h"

#include <vector>


#pragma warning( disable : 4251 )	//export std

AK_NAMESPACE_BEGIN

enum class EBindingType
{
	EBindingType_Uniform,
	EBindingType_Sampler,
	EBindingType_Storage
};

enum class EDescriptorStage
{
	EDescriptorStage_Vertex,
	EDescriptorStage_TessellationControl,
	EDescriptorStage_TessellationEvaluation,
	EDescriptorStage_Geometry,
	EDescriptorStage_Fragment,
	EDescriptorStage_Compute
};

class Device;
class AK_DLL DescriptorSetLayout final
{
public:
	DescriptorSetLayout(SharedPtr<Device> _device);
	~DescriptorSetLayout();

	bool Create();

	AK_INLINE const VkDescriptorSetLayout* GetDescriptorLayoutPtr() const
	{
		return &m_descriptorSetLayout;
	}

	AK_INLINE const VkDescriptorSetLayout& GetDescriptorLayout() const
	{
		return m_descriptorSetLayout;
	}

	void Push(EDescriptorStage _stage, EBindingType _type, uint32_t _index);

	static VkShaderStageFlags ConvertDescriptorStage(EDescriptorStage _stage);
	static VkDescriptorType ConvertBindingType(EBindingType _type);

private:
	SharedPtr<Device> m_device;
	VkDescriptorSetLayout m_descriptorSetLayout;
	std::vector<VkDescriptorSetLayoutBinding> m_bindings;
};

AK_NAMESPACE_END