#include "StagingManager.h"

#include "Device.h"
#include "SwapChain.h"

#include "../Core/Assertions.h"
#include "../Core/VulkanAllocator.h"



AK_NAMESPACE_BEGIN

StagingManager& StagingManager::Instance()
{
	static StagingManager instance;
	return instance;
}

StagingManager::StagingManager() :
	m_maxBufferSize(0),
	m_currentBuffer(0),
	m_numFrames(0),
	m_mappedData(nullptr),
	m_memory(VK_NULL_HANDLE),
	m_commandPool(VK_NULL_HANDLE),
	m_allocationInfo({}),
	m_allocation(nullptr)
{

}

StagingManager::~StagingManager()
{

}

bool StagingManager::Init(SharedPtr<Device> _device, SharedPtr<SwapChain> _swapChain, uint32_t _size /*= AK_MEMORY_64_MB*/)
{
	m_device = _device;
	m_numFrames = _swapChain->GetImageViewsCount();
	m_maxBufferSize = _size * (uint32_t)m_numFrames;

	m_buffers.resize(m_numFrames);

	VkBufferCreateInfo bufferCreateInfo = {};
	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.size = _size;
	bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

	VmaAllocationCreateInfo allocInfo = {};
	allocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;	// VMA_MEMORY_USAGE_GPU_ONLY;
	allocInfo.requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
	allocInfo.preferredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	allocInfo.flags = VMA_ALLOCATION_CREATE_DONT_BIND_BIT;		// VMA_ALLOCATION_CREATE_MAPPED_BIT

	VkResult result;
	for (size_t i = 0; i < m_numFrames; ++i)
	{
		result = vmaCreateBuffer(VulkanAllocator::Instance().GetVMA(), &bufferCreateInfo, &allocInfo, &m_buffers[i].m_buffer, &m_buffers[i].m_allocation, &m_buffers[i].m_allocationInfo);
		akAssertReturnValue(result == VK_SUCCESS, false, "Cannot create buffer");
	}

	VkMemoryRequirements memoryRequirements;
	vkGetBufferMemoryRequirements(m_device->GetDevice(), m_buffers[0].m_buffer, &memoryRequirements);

	memoryRequirements.size = m_maxBufferSize;
	const VkDeviceSize alignMod = _size % memoryRequirements.alignment;
	const VkDeviceSize alignedSize = (alignMod == 0) ? _size : (_size + memoryRequirements.alignment - alignMod);


	allocInfo = {};
	allocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;	// VMA_MEMORY_USAGE_GPU_ONLY;

	result = vmaAllocateMemory(VulkanAllocator::Instance().GetVMA(), &memoryRequirements, &allocInfo, &m_allocation, &m_allocationInfo);
	akAssertReturnValue(result == VK_SUCCESS, false, "Cannot allocate memory");

	for (size_t i = 0; i < m_numFrames; ++i)
	{
		result = vmaBindBufferMemory2(VulkanAllocator::Instance().GetVMA(), m_allocation, i * alignedSize, m_buffers[i].m_buffer, nullptr);
		akAssertReturnValue(result == VK_SUCCESS, false, "Cannot bind buffer to memory");
	}

	result = vmaMapMemory(VulkanAllocator::Instance().GetVMA(), m_allocation, reinterpret_cast<void**>(&m_mappedData));
	akAssertReturnValue(result == VK_SUCCESS, false, "Cannot map the memory");


	VkCommandPoolCreateInfo commandPoolCreateInfo = {};
	commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	commandPoolCreateInfo.queueFamilyIndex = m_device->GetQueueFamily()->GetGraphicsFamily();
	result = vkCreateCommandPool(m_device->GetDevice(), &commandPoolCreateInfo, VulkanAllocator::Instance().GetCallbacks(), &m_commandPool);
	akAssertReturnValue(result == VK_SUCCESS, false, "Cannot create command pool");

	VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
	commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufferAllocateInfo.commandPool = m_commandPool;
	commandBufferAllocateInfo.commandBufferCount = 1;

	VkFenceCreateInfo fenceCreateInfo = {};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

	VkCommandBufferBeginInfo commandBufferBeginInfo = {};
	commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	for (int i = 0; i < m_numFrames; ++i)
	{
		result = vkAllocateCommandBuffers(m_device->GetDevice(), &commandBufferAllocateInfo, &m_buffers[i].m_commandBuffer);
		akAssertReturnValue(result == VK_SUCCESS, false, "Cannot allocate command buffer");

		result = vkCreateFence(m_device->GetDevice(), &fenceCreateInfo, VulkanAllocator::Instance().GetCallbacks(), &m_buffers[i].m_fence);
		akAssertReturnValue(result == VK_SUCCESS, false, "Cannot create fence");

		result = vkBeginCommandBuffer(m_buffers[i].m_commandBuffer, &commandBufferBeginInfo);
		akAssertReturnValue(result == VK_SUCCESS, false, "Cannot begin command buffer");
	}

	return true;
}

void StagingManager::Shutdown()
{
	vmaUnmapMemory(VulkanAllocator::Instance().GetVMA(), m_allocation);
	m_memory = VK_NULL_HANDLE;
	m_mappedData = nullptr;

	for (uint32_t i = 0; i < m_numFrames; ++i)
	{
		vkDestroyFence(m_device->GetDevice(), m_buffers[i].m_fence, VulkanAllocator::Instance().GetCallbacks());
		vmaDestroyBuffer(VulkanAllocator::Instance().GetVMA(), m_buffers[i].m_buffer, m_buffers[i].m_allocation);
		vkFreeCommandBuffers(m_device->GetDevice(), m_commandPool, 1, &m_buffers[i].m_commandBuffer);
	}

	vkDestroyCommandPool(m_device->GetDevice(), m_commandPool, VulkanAllocator::Instance().GetCallbacks());

	vmaFreeMemory(VulkanAllocator::Instance().GetVMA(), m_allocation);

	m_buffers.clear();

	m_device.reset();

	m_maxBufferSize = 0;
	m_currentBuffer = 0;
}

uint8_t* StagingManager::Stage(size_t _size, size_t _alignment, VkCommandBuffer& _commandBuffer, VkBuffer& _buffer, size_t& _bufferOffset)
{
	akAssertReturnValue(_size < m_maxBufferSize, nullptr, "Size is outbound of total memory!");

	StagingBuffer* stage = &m_buffers[m_currentBuffer];
	const size_t alignMod = stage->m_allocationInfo.offset % _alignment;
	stage->m_allocationInfo.offset = ((stage->m_allocationInfo.offset % _alignment) == 0) ? stage->m_allocationInfo.offset : (stage->m_allocationInfo.offset + _alignment - alignMod);

	if ((stage->m_allocationInfo.offset + _size) >= (m_maxBufferSize) && !stage->m_submitted)
	{
		Flush();
	}

	// can change the current buffer, so I need to get it again
	stage = &m_buffers[m_currentBuffer];
	if (stage->m_submitted)
	{
		Wait(*stage);
	}

	_commandBuffer = stage->m_commandBuffer;
	_buffer = stage->m_buffer;
	_bufferOffset = stage->m_allocationInfo.offset;

	uint8_t* data = reinterpret_cast<uint8_t*>(stage->m_allocationInfo.pMappedData) + stage->m_allocationInfo.offset;
	stage->m_allocationInfo.offset += _size;

	return data;
}

void StagingManager::Flush()
{
	StagingBuffer& stage = m_buffers[m_currentBuffer];
	if (stage.m_submitted || stage.m_allocationInfo.offset == 0)
	{
		return;
	}

	VkMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
	barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	barrier.dstAccessMask = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT | VK_ACCESS_INDEX_READ_BIT;
	vkCmdPipelineBarrier(
		stage.m_commandBuffer,
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
		0, 1, &barrier, 0, NULL, 0, NULL);

	vkEndCommandBuffer(stage.m_commandBuffer);

	VkMappedMemoryRange memoryRange = {};
	memoryRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
	memoryRange.memory = m_memory;
	memoryRange.size = VK_WHOLE_SIZE;
	vkFlushMappedMemoryRanges(m_device->GetDevice(), 1, &memoryRange);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &stage.m_commandBuffer;

	vkQueueSubmit(m_device->GetGraphicsQueue(), 1, &submitInfo, stage.m_fence);

	stage.m_submitted = true;

	m_currentBuffer = (m_currentBuffer + (uint32_t)1) % (uint32_t)m_numFrames;
}

void StagingManager::Wait(StagingBuffer& _stage)
{
	if (_stage.m_submitted == false)
	{
		return;
	}

	VkResult result = vkWaitForFences(m_device->GetDevice(), 1, &_stage.m_fence, VK_TRUE, UINT64_MAX);
	akAssertReturnVoid(result == VK_SUCCESS, "Cannot wait for fences");

	result = vkResetFences(m_device->GetDevice(), 1, &_stage.m_fence);
	akAssertReturnVoid(result == VK_SUCCESS, "Cannot reset fences");

	_stage.m_allocationInfo.offset = 0;
	_stage.m_submitted = false;

	VkCommandBufferBeginInfo commandBufferBeginInfo = {};
	commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	result = vkBeginCommandBuffer(_stage.m_commandBuffer, &commandBufferBeginInfo);
	akAssertReturnVoid(result == VK_SUCCESS, "Cannot begin command bufer");
}

AK_NAMESPACE_END