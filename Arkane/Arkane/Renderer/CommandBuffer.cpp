#include "CommandBuffer.h"

#include "../Core/Assertions.h"
#include "../Core/Defines.h"
#include "../Core/SmartPointers.h"
#include "../Core/VulkanAllocator.h"

#include "CommandPool.h"
#include "RenderPass.h"
#include "PipelineLayout.h"
#include "Pipeline.h"
#include "Device.h"
#include "DescriptorSet.h"

#include "Buffers/BaseBufferObject.h"
#include "Buffers/VertexBufferObject.h"
#include "Buffers/IndexBufferObject.h"


AK_NAMESPACE_BEGIN


CommandBuffer::CommandBuffer(SharedPtr<Device> _device, SharedPtr<CommandPool> _pool, VkCommandBufferLevel _level /*= VK_COMMAND_BUFFER_LEVEL_PRIMARY*/)
	: m_device(_device)
	, m_pool(_pool)
	, m_commandBuffer(VK_NULL_HANDLE)
{
	VkCommandBufferAllocateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	createInfo.commandPool = m_pool->GetCommandPool();
	createInfo.level = _level;
	createInfo.commandBufferCount = 1;

	VkResult result = vkAllocateCommandBuffers(m_device->GetDevice(), &createInfo, &m_commandBuffer);
	akAssertReturnVoid(result == VK_SUCCESS, "Cannot create CommandBuffer");
}

CommandBuffer::~CommandBuffer()
{
	if (m_device != VK_NULL_HANDLE && m_commandBuffer != VK_NULL_HANDLE)
	{
		vkFreeCommandBuffers(m_device->GetDevice(), m_pool->GetCommandPool(), 1, &m_commandBuffer);
	}

	m_device.reset();
	m_pool.reset();
}

bool CommandBuffer::Begin()
{
	VkCommandBufferBeginInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	VkResult result = vkBeginCommandBuffer(m_commandBuffer, &createInfo);
	akAssertReturnValue(result == VK_SUCCESS, false, "Cannot begin the CommandBuffer");

	return true;
}

bool CommandBuffer::End()
{
	VkResult result = vkEndCommandBuffer(m_commandBuffer);
	akAssertReturnValue(result == VK_SUCCESS, false, "Cannot end the CommandBuffer");

	return true;
}

void CommandBuffer::SetViewport(float _x, float _y, float _width, float _height, float _minDepth, float _maxDepth)
{
	VkViewport viewport = { _x, _y, _width, _height, _minDepth, _maxDepth };
	vkCmdSetViewport(m_commandBuffer, 0, 1, &viewport);
}

void CommandBuffer::SetScissor(int32_t _offsetx, int32_t _offsety, uint32_t _width, uint32_t _height)
{
	VkRect2D scissor = { _offsetx, _offsety, _width, _height };
	vkCmdSetScissor(m_commandBuffer, 0, 1, &scissor);
}

void CommandBuffer::BeginRenderPass(SharedPtr<RenderPass> _renderPass, VkFramebuffer _frameBuffer)
{
	_renderPass->SetFrameBuffer(_frameBuffer);
	VkRenderPassBeginInfo beginInfo = _renderPass->GetBeginInfo();

	vkCmdBeginRenderPass(m_commandBuffer, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void CommandBuffer::EndRenderPass()
{
	vkCmdEndRenderPass(m_commandBuffer);
}

void CommandBuffer::BindDescriptorSet(SharedPtr<PipelineLayout> _layout, SharedPtr<DescriptorSet> _descriptorSet)
{
	vkCmdBindDescriptorSets(m_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _layout->GetLayout(), 0, 1, _descriptorSet->GetDescriptorSetPtr(), 0, nullptr);
}

void CommandBuffer::BindDescriptorSets(SharedPtr<PipelineLayout> _layout, std::vector<SharedPtr<DescriptorSet>> _descriptorSet, uint32_t firstSet /*= 0*/)
{
	std::vector<VkDescriptorSet> descSets;
	for (auto set : _descriptorSet)
	{
		descSets.push_back(set->GetDescriptorSet());
	}
	vkCmdBindDescriptorSets(m_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _layout->GetLayout(), firstSet, static_cast<uint32_t>(descSets.size()), descSets.data(), 0, nullptr);
}

void CommandBuffer::BindPipeline(SharedPtr<Pipeline> _pipeline)
{
	vkCmdBindPipeline(m_commandBuffer, _pipeline->GetPipelineBindingPoint(), _pipeline->GetPipeline());
}

void CommandBuffer::BindVertexBuffer(SharedPtr<VertexBufferObject> _buffer, uint32_t _binding, VkDeviceSize _offset /*= 0*/)
{
	vkCmdBindVertexBuffers(m_commandBuffer, _binding, 1, _buffer->GetBufferObjectPtr(), &_offset);
}

void CommandBuffer::BindIndexBuffer(SharedPtr<IndexBufferObject> _buffer, VkIndexType _indexType, VkDeviceSize _offset /*= 0*/)
{
	vkCmdBindIndexBuffer(m_commandBuffer, _buffer->GetBufferObject(), _offset, _indexType);
}

void CommandBuffer::Draw(uint32_t _vertexCount, uint32_t _instanceCount, uint32_t _firstVertex, uint32_t _firstInstance)
{
	vkCmdDraw(m_commandBuffer, _vertexCount, _instanceCount, _firstVertex, _firstInstance);
}

void CommandBuffer::DrawIndexed(uint32_t _indexCount, uint32_t _instanceCount, uint32_t _firstIndex, uint32_t _vertexOffset, uint32_t _firstInstance)
{
	vkCmdDrawIndexed(m_commandBuffer, _indexCount, _instanceCount, _firstIndex, _vertexOffset, _firstInstance);
}

void CommandBuffer::UpdatePushConstant(SharedPtr<PipelineLayout> _layout, uint32_t _index, const void* _values)
{
	VkPushConstantRange pushConstantRange = _layout->GetPushConstant(_index);
	vkCmdPushConstants(m_commandBuffer, _layout->GetLayout(), pushConstantRange.stageFlags, pushConstantRange.offset, pushConstantRange.size, _values);
}

AK_NAMESPACE_END