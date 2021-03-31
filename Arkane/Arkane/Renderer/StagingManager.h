#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Core/Defines.h"
#include "../Core/SmartPointers.h"

#include "../Dependencies/VulkanMemoryAllocator/src/vk_mem_alloc.h"

#include "QueueFamily.h"

#include <vector>


#pragma warning(disable:4251)


AK_NAMESPACE_BEGIN


class Device;
class SwapChain;
struct StagingBuffer
{
	StagingBuffer() :
		m_submitted(false),
		m_commandBuffer(VK_NULL_HANDLE),
		m_buffer(VK_NULL_HANDLE),
		m_fence(VK_NULL_HANDLE),
		m_allocationInfo({}),
		m_allocation(nullptr) {}

	bool m_submitted;

	VkCommandBuffer m_commandBuffer;
	VkBuffer m_buffer;
	VkFence m_fence;

	VmaAllocation m_allocation;
	VmaAllocationInfo m_allocationInfo;
};


class AK_DLL StagingManager final
{
public:
	static StagingManager& GetInstance();

	bool Init(SharedPtr<Device> _device, SharedPtr<SwapChain> _swapChain, uint32_t _size = AK_MEMORY_64_MB);
	void Shutdown();

	uint8_t* Stage(size_t _size, size_t _alignment, VkCommandBuffer& _commandBuffer, VkBuffer& _buffer, size_t& _bufferOffset);

	void Flush();	// MUST be called at the beginning of the frame


private:
	StagingManager();
	~StagingManager();

	void Wait(StagingBuffer& _stage);

private:
	SharedPtr<Device> m_device;

	size_t m_numFrames;
	uint32_t m_maxBufferSize;
	uint32_t m_currentBuffer;
	int8_t* m_mappedData;

	VmaAllocation m_allocation;
	VmaAllocationInfo m_allocationInfo;

	VkCommandPool m_commandPool;

	std::vector<StagingBuffer> m_buffers;
};

AK_NAMESPACE_END
