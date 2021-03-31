#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Core/Defines.h"
#include "../Core/SmartPointers.h"

#include <vector>


#pragma warning(disable : 4251)


AK_NAMESPACE_BEGIN

class Device;
class Pipeline;
class PipelineLayout;
class RenderPass;
class CommandPool;
class VertexBufferObject;
class IndexBufferObject;
class DescriptorSet;
class AK_DLL CommandBuffer final
{
public:
	CommandBuffer(SharedPtr<Device> _device, SharedPtr<CommandPool> _pool, VkCommandBufferLevel _level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
	~CommandBuffer();


	AK_INLINE const VkCommandBuffer& GetCommandBuffer() const
	{
		return m_commandBuffer;
	}

	AK_INLINE const VkCommandBuffer* GetCommandBufferPtr() const
	{
		return &m_commandBuffer;
	}

	bool Begin();
	bool End();
	void BeginRenderPass(SharedPtr<RenderPass> _renderPass, VkFramebuffer _frameBuffer);
	void EndRenderPass();
	void BindDescriptorSet(SharedPtr<PipelineLayout> _layout, SharedPtr<DescriptorSet> _descriptorSet);
	void BindDescriptorSets(SharedPtr<PipelineLayout> _layout, std::vector<SharedPtr<DescriptorSet>> _descriptorSet, uint32_t firstSet = 0);
	void SetViewport(float _x, float _y, float _width, float _height, float _minDepth, float _maxDepth);
	void SetScissor(int32_t _offsetx, int32_t _offsety, uint32_t _width, uint32_t _height);
	void BindPipeline(SharedPtr<Pipeline> _pipeline);
	void BindVertexBuffer(SharedPtr<VertexBufferObject> _buffer, uint32_t _binding, VkDeviceSize _offset = 0);
	void BindIndexBuffer(SharedPtr<IndexBufferObject> _buffer, VkIndexType _indexType, VkDeviceSize _offset = 0);
	void Draw(uint32_t _vertexCount, uint32_t _instanceCount, uint32_t _firstVertex, uint32_t _firstInstance);
	void DrawIndexed(uint32_t _indexCount, uint32_t _instanceCount, uint32_t _firstIndex, uint32_t vertexOffset, uint32_t _firstInstance);
	void UpdatePushConstant(SharedPtr<PipelineLayout> _layout, uint32_t _index, const void* _values);

private:
	SharedPtr<Device> m_device;
	SharedPtr<CommandPool> m_pool;

	VkCommandBuffer m_commandBuffer;
};

AK_NAMESPACE_END