#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Core/Defines.h"


AK_NAMESPACE_BEGIN

class AK_DLL VulkanAllocator final
{
public:
	static VulkanAllocator& Instance();

	AK_INLINE const VkAllocationCallbacks* GetCallbacks() const { return &m_callback; }

private:
	static void* VKAPI_CALL Allocation(void* _pUserData, size_t _size, size_t _alignment, VkSystemAllocationScope _allocationScope);
	static void* VKAPI_CALL Reallocation(void* _pUserData, void* _pOriginal, size_t _size, size_t _alignment, VkSystemAllocationScope _allocationScope);
	static void VKAPI_CALL Free(void* _pUserData, void* _pMemory);

	void* Allocation(size_t _size, size_t _alignment, VkSystemAllocationScope _allocationScope);
	void* Reallocation(void* _pOriginal, size_t _size, size_t _alignment, VkSystemAllocationScope _allocationScope);
	void Free(void* _pMemory);

	VulkanAllocator()
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
	VkAllocationCallbacks m_callback;
};

AK_NAMESPACE_END