#include "VulkanAllocator.h"

#include "../Dependencies/VMem/VMem.h"

#define VMA_IMPLEMENTATION
#include  "../Dependencies/VulkanMemoryAllocator/src/vk_mem_alloc.h"

#include "../Core/Assertions.h"

#include "../Renderer/Device.h"
#include "../Renderer/Instance.h"


#pragma warning(disable : 26812)


AK_NAMESPACE_BEGIN

VulkanAllocator& VulkanAllocator::GetInstance()
{
	static VulkanAllocator instance;
	return instance;
}

void VulkanAllocator::CreateVMA(SharedPtr<Device> _device)
{
	VmaAllocatorCreateInfo allocatorInfo = {};
	allocatorInfo.vulkanApiVersion = Instance::s_ApiVersion;
	allocatorInfo.physicalDevice = _device->GetPhysicalDevice();
	allocatorInfo.device = _device->GetDevice();
	allocatorInfo.instance = _device->GetInstance();

	vmaCreateAllocator(&allocatorInfo, &m_vmaAllocator);
}

void VulkanAllocator::DestroyVMA()
{
	vmaDestroyAllocator(m_vmaAllocator);
}

void* VulkanAllocator::Allocation(size_t _size, size_t _alignment, VkSystemAllocationScope _allocationScope)
{
	void* p = VMem::AllocAligned(_size, _alignment);
	return p;
}

void* VKAPI_CALL VulkanAllocator::Allocation(void* _pUserData, size_t _size, size_t _alignment, VkSystemAllocationScope _allocationScope)
{
	return static_cast<VulkanAllocator*>(_pUserData)->Allocation(_size, _alignment, _allocationScope);
}

void* VulkanAllocator::Reallocation(void* _pOriginal, size_t _size, size_t _alignment, VkSystemAllocationScope _allocationScope)
{
	void* p = VMem::ReallocAligned(_pOriginal, _size, _alignment);
	return p;
}

void* VKAPI_CALL VulkanAllocator::Reallocation(void* _pUserData, void* _pOriginal, size_t _size, size_t _alignment, VkSystemAllocationScope _allocationScope)
{
	return static_cast<VulkanAllocator*>(_pUserData)->Reallocation(_pOriginal, _size, _alignment, _allocationScope);
}

void VulkanAllocator::Free(void* _pMemory)
{
	if (_pMemory == nullptr) // https://www.khronos.org/registry/vulkan/specs/1.0/man/html/PFN_vkFreeFunction.html (may be null and it would be safe anyway!)
	{
		return;
	}

	VMem::FreeAligned(_pMemory);
}

void VKAPI_CALL VulkanAllocator::Free(void* _pUserData, void* _pMemory)
{
	return static_cast<VulkanAllocator*>(_pUserData)->Free(_pMemory);
}

AK_NAMESPACE_END