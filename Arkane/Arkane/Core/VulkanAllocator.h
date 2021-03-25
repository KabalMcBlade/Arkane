#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Core/Defines.h"
#include "../Core/SmartPointers.h"

#include  "../Dependencies/VulkanMemoryAllocator/src/vk_mem_alloc.h"


#pragma warning(disable:4251)

AK_NAMESPACE_BEGIN

class Device;
class AK_DLL VulkanAllocator final
{
public:
	static VulkanAllocator& Instance();

	void CreateVMA(SharedPtr<Device> _device);
	void DestroyVMA();

	AK_INLINE const VkAllocationCallbacks* GetCallbacks() const { return &m_callback; }
	AK_INLINE const VmaAllocator& GetVMA() const { return m_vmaAllocator; }

private:
	static void* VKAPI_CALL Allocation(void* _pUserData, size_t _size, size_t _alignment, VkSystemAllocationScope _allocationScope);
	static void* VKAPI_CALL Reallocation(void* _pUserData, void* _pOriginal, size_t _size, size_t _alignment, VkSystemAllocationScope _allocationScope);
	static void VKAPI_CALL Free(void* _pUserData, void* _pMemory);

	void* Allocation(size_t _size, size_t _alignment, VkSystemAllocationScope _allocationScope);
	void* Reallocation(void* _pOriginal, size_t _size, size_t _alignment, VkSystemAllocationScope _allocationScope);
	void Free(void* _pMemory);

	VulkanAllocator() : m_vmaAllocator(nullptr)
	{
		m_callback.pUserData = (void*)this;
		m_callback.pfnAllocation = &VulkanAllocator::Allocation;
		m_callback.pfnReallocation = &VulkanAllocator::Reallocation;
		m_callback.pfnFree = &VulkanAllocator::Free;
		m_callback.pfnInternalAllocation = nullptr;
		m_callback.pfnInternalFree = nullptr;
	}

	~VulkanAllocator() {}

private:
	VmaAllocator m_vmaAllocator;
	VkAllocationCallbacks m_callback;
};

AK_NAMESPACE_END