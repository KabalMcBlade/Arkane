#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Core/Defines.h"
#include "../Core/SmartPointers.h"

#include <vector>


#pragma warning( disable : 4251 )	//export std


AK_NAMESPACE_BEGIN

class DescriptorPool;
class DescriptorSetLayout;
class Device;
class AK_DLL DescriptorSet final
{
public:
	DescriptorSet(SharedPtr<Device> _device);
	~DescriptorSet();

	bool Create(SharedPtr<DescriptorPool> _descriptorPool);

	AK_INLINE const VkDescriptorSet& GetDescriptorSet() const
	{
		return m_descriptorSet;
	}

	AK_INLINE const VkDescriptorSet* GetDescriptorSetPtr() const
	{
		return &m_descriptorSet;
	}

	void PushLayout(SharedPtr<DescriptorSetLayout> _layout);
	void PushDescriptor(VkWriteDescriptorSet _descriptor);
	void PushDescriptor(uint32_t _binding, VkDescriptorType _type, const VkDescriptorBufferInfo* _bufferInfo, uint32_t _descriptorCount = 1);
	void PushDescriptor(uint32_t _binding, VkDescriptorType _type, const VkDescriptorImageInfo* _imageInfo, uint32_t _descriptorCount = 1);

private:
	SharedPtr<Device> m_device;

	VkDescriptorSet m_descriptorSet;

	std::vector<VkDescriptorSetLayout> m_layouts;
	std::vector<VkWriteDescriptorSet> m_descriptors;
};

AK_NAMESPACE_END