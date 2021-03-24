#include "PipelineCache.h"

#include "../Core/Assertions.h"
#include "../Core/VulkanAllocator.h"

#include "Device.h"


AK_NAMESPACE_BEGIN

PipelineCache::PipelineCache(SharedPtr<Device> _device) : m_device(_device)
{
	VkPipelineCacheCreateInfo pipelineCacheCreateInfo{};
	pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
	VkResult result = vkCreatePipelineCache(m_device->GetDevice(), &pipelineCacheCreateInfo, VulkanAllocator::Instance().GetCallbacks(), &m_pipelineCache);
	akAssertReturnVoid(result == VK_SUCCESS, "Cannot generate Pipeline!");
}

PipelineCache::~PipelineCache()
{
	if (m_device != VK_NULL_HANDLE && m_pipelineCache != VK_NULL_HANDLE)
	{
		vkDestroyPipelineCache(m_device->GetDevice(), m_pipelineCache, VulkanAllocator::Instance().GetCallbacks());
	}
}

AK_NAMESPACE_END