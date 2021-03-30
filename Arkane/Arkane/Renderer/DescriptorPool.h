#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>


#include "../Core/Defines.h"
#include "../Core/SmartPointers.h"

#include <vector>


#pragma warning( disable : 4251 )	//export std

AK_NAMESPACE_BEGIN

class Device;
class AK_DLL DescriptorPool final
{
public:
	DescriptorPool(SharedPtr<Device> _device);
	~DescriptorPool();

	bool Create(VkDescriptorPoolCreateFlags _flag = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT, uint32_t _maxDescriptorSet = 16384);

	AK_INLINE const VkDescriptorPool& GetDescritorPool() const
	{
		return m_descriptorPool;
	}

	void Push(VkDescriptorType _type, uint32_t _descriptorCount);

private:
	SharedPtr<Device> m_device;

	VkDescriptorPool m_descriptorPool;

	std::vector<VkDescriptorPoolSize> m_poolSizes;
};

AK_NAMESPACE_END