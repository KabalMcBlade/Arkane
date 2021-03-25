#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../../Core/Defines.h"

#include "../../Dependencies/VulkanMemoryAllocator/src/vk_mem_alloc.h"


AK_NAMESPACE_BEGIN

enum class EBufferMappingType
{
	EBufferMappingType_Read,
	EBufferMappingType_Write
};

enum class EBufferUsage
{
	EBufferUsage_Static,	// GPU Read
	EBufferUsage_Dynamic,	// GPU Read, CPU Read/Write
};

class BaseBufferObject
{
public:
	BaseBufferObject() : m_size(0), m_offsetInOtherBuffer(0), m_usage(EBufferUsage::EBufferUsage_Dynamic), m_apiObject(VK_NULL_HANDLE), m_vmaAllocation(nullptr), m_allocation({})  {}
	~BaseBufferObject() {}

	AK_INLINE int32_t GetSize() const { return (m_size & ~kMappedFlag); }
	int32_t GetAllocedSize() const { return ((m_size & ~kMappedFlag) + 15) & ~15; }
	EBufferUsage GetUsage() const { return m_usage; }
	VkBuffer GetAPIObject() const { return m_apiObject; }
	int32_t GetOffset() const { return (m_offsetInOtherBuffer & ~kOwnedFlag); }

	bool IsMapped() const { return (m_size & kMappedFlag) != 0; }

protected:
	void SetMapped() const { const_cast<int32_t&>(m_size) |= kMappedFlag; }
	void SetUnmapped() const { const_cast<int32_t&>(m_size) &= ~kMappedFlag; }
	bool OwnsBuffer() const { return ((m_offsetInOtherBuffer & kOwnedFlag) != 0); }

protected:
	int32_t m_size;					// size in bytes
	int32_t m_offsetInOtherBuffer;	// offset in bytes
	EBufferUsage m_usage;

	VkBuffer m_apiObject;

	VmaAllocation m_vmaAllocation;
	VmaAllocationInfo m_allocation;

	static constexpr int kMappedFlag = 1 << 31;
	static constexpr int kOwnedFlag = 1 << 31;
};

AK_NAMESPACE_END
