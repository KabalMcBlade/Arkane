#include "VertexBufferObject.h"

#include "../../Core/Assertions.h"
#include "../../Core/MemCpy.h"
#include "../../Core/VulkanAllocator.h"

#include "../../Renderer/StagingManager.h"


AK_NAMESPACE_BEGIN

VertexBufferObject::VertexBufferObject()
{
	SetUnmapped();
}

VertexBufferObject::~VertexBufferObject()
{

}

bool VertexBufferObject::AllocBufferObject(const void* _data, uint32_t _allocSize, EBufferUsage _usage)
{
	akAssertReturnValue(m_apiObject == VK_NULL_HANDLE, false, "BufferObject already allocated");
	akAssertReturnValue(AK_IS_ALIGNED(_data, 16), false, "Buffer not aligned to 16");
	akAssertReturnValue(_allocSize > 0, false, "Size must be greeter than 0");

	m_size = _allocSize;
	m_usage = _usage;

	uint32_t numBytes = GetAllocedSize();

	VkBufferCreateInfo bufferCreateInfo = {};
	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.pNext = nullptr;
	bufferCreateInfo.size = numBytes;
	bufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	if (m_usage == EBufferUsage::EBufferUsage_Static)
	{
		bufferCreateInfo.usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	}

	VmaAllocationCreateInfo allocInfo = {};
	if (m_usage == EBufferUsage::EBufferUsage_Static)
	{
		allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
	}
	else if (m_usage == EBufferUsage::EBufferUsage_Dynamic)
	{
		allocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
		allocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
	}

	VkResult result = vmaCreateBuffer(VulkanAllocator::Instance().GetVMA(), &bufferCreateInfo, &allocInfo, &m_apiObject, &m_vmaAllocation, &m_allocation);
	akAssertReturnValue(result == VK_SUCCESS, false, "Cannot create buffer");

	if (_data != nullptr) 
	{
		Update(_data, _allocSize);
	}

	return true;
}

void VertexBufferObject::FreeBufferObject()
{
	if (IsMapped())
	{
		UnmapBuffer();
	}

	if (OwnsBuffer() == false) 
	{
		ClearWithoutFreeing();
		return;
	}

	if (m_apiObject == VK_NULL_HANDLE)
	{
		return;
	}

	if (m_apiObject != VK_NULL_HANDLE) 
	{
		vmaDestroyBuffer(VulkanAllocator::Instance().GetVMA(), m_apiObject, m_vmaAllocation);
		m_apiObject = VK_NULL_HANDLE;
		m_allocation = VmaAllocationInfo();
		m_vmaAllocation = nullptr;
	}

	ClearWithoutFreeing();
}

void VertexBufferObject::Reference(const VertexBufferObject& _other)
{
	akAssertReturnVoid(!IsMapped(), "Buffer must be not mapped");
	akAssertReturnVoid(_other.GetSize() > 0, "The other buffer must greater than 0");

	FreeBufferObject();

	m_size = _other.GetSize();					// this strips the kMappedFlag
	m_offsetInOtherBuffer = _other.GetOffset();	// this strips the kOwnedFlag
	m_usage = _other.m_usage;
	m_apiObject = _other.m_apiObject;

	m_allocation = _other.m_allocation;

	akAssertReturnVoid(!OwnsBuffer(), "Should not own this buffer now!");
}

void VertexBufferObject::Reference(const VertexBufferObject& _other, uint32_t _refOffset, uint32_t _refSize)
{
	akAssertReturnVoid(!IsMapped(), "Buffer must be not mapped");
	akAssertReturnVoid(_refOffset >= 0, "Offset must be greater or equal to 0");
	akAssertReturnVoid(_refSize >= 0, "Size must be greater or equal to 0");
	akAssertReturnVoid(_refOffset + _refSize <= (uint32_t)_other.GetSize(), "Size plus offset must be less or equal to other size");

	FreeBufferObject();

	m_size = _refSize;
	m_offsetInOtherBuffer = _other.GetOffset() + _refOffset;
	m_usage = _other.m_usage;
	m_apiObject = _other.m_apiObject;
	m_allocation = _other.m_allocation;

	akAssertReturnVoid(!OwnsBuffer(), "Should not own this buffer now!");
}

void VertexBufferObject::Update(const void* _data, uint32_t _size, uint32_t _offset /*= 0*/) const
{
	akAssertReturnVoid(m_apiObject != VK_NULL_HANDLE, "BufferObject is not allocated");
	akAssertReturnVoid(AK_IS_ALIGNED(_data, 16), "Buffer not aligned to 16");
	akAssertReturnVoid((GetOffset() & 15) == 0, "Offset not aligned to 16");
	akAssertReturnVoid(_size < (uint32_t)GetSize(), "Size must be less of the total size than 0");

	if (m_usage == EBufferUsage::EBufferUsage_Dynamic)
	{
		MemCpy((uint8_t*)m_allocation.pMappedData + GetOffset() + _offset, (const uint8_t*)_data, _size);
	}
	else 
	{
		VkBuffer stageBuffer;
		VkCommandBuffer commandBuffer;
		size_t stageOffset = 0;
		uint8_t* stageData = StagingManager::Instance().Stage(_size, 1, commandBuffer, stageBuffer, stageOffset);

		MemCpy(stageData, _data, _size);

		VkBufferCopy bufferCopy = {};
		bufferCopy.srcOffset = stageOffset;
		bufferCopy.dstOffset = (GetOffset() + _offset);
		bufferCopy.size = _size;

		vkCmdCopyBuffer(commandBuffer, stageBuffer, m_apiObject, 1, &bufferCopy);
	}
}

void* VertexBufferObject::MapBuffer(EBufferMappingType _mapType)
{
	akAssertReturnValue(m_apiObject != VK_NULL_HANDLE, nullptr, "Buffer must be allocated");
	akAssertReturnValue(m_usage != EBufferUsage::EBufferUsage_Static, nullptr, "Cannot map static buffer");

	void* buffer = (uint8_t*)m_allocation.pMappedData + GetOffset();

	SetMapped();

	akAssertReturnValue(buffer != nullptr, nullptr, "Failed to map the buffer");

	return buffer;
}

void VertexBufferObject::UnmapBuffer()
{
	akAssertReturnVoid(m_apiObject != VK_NULL_HANDLE, "Buffer must be allocated");
	akAssertReturnVoid(m_usage != EBufferUsage::EBufferUsage_Static, "Cannot unmap static buffer");

	SetUnmapped();
}

void VertexBufferObject::ClearWithoutFreeing()
{
	m_size = 0;
	m_offsetInOtherBuffer = kOwnedFlag;
	m_apiObject = VK_NULL_HANDLE;
	m_allocation = VmaAllocationInfo();
	m_vmaAllocation = nullptr;
}

AK_NAMESPACE_END