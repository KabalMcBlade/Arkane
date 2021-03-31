#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Core/Defines.h"
#include "../Core/SmartPointers.h"

#include "Frame.h"	// to have enum type visible

#include <string>
#include <map>
#include <memory>
#include <string>
#include <vector>


#pragma warning(disable : 4251)


AK_NAMESPACE_BEGIN

class Instance;
class Device;
class SwapChain;
class RenderPass;
class Pipeline;
class PipelineCache;
class FrameBuffer;
class CommandPool;
class CommandBuffer;
class VertexBufferObject;
class IndexBufferObject;
class DescriptorSetLayout;
class PipelineLayout;
class VertexDescriptor;
class Shader;

class AK_DLL RenderManager final
{
public:
	static RenderManager& GetInstance();

	//////////////////////////////////////////////////////////////////////////
	// Main functions and methods
	void CreateInstance(const char* _appName, uint32_t _appVersion = VK_MAKE_VERSION(0, 0, 1));
	void CreateCore(const VkSurfaceKHR& _surface, int32_t _frameWidth, int32_t _frameHeight, VkPhysicalDeviceFeatures _enabledFeatures);
	void CreateBuffers(int32_t _frameWidth, int32_t _frameHeight, uint32_t _layers = 1);
	void CreateFrame();

	EFrameStatus BeginFrame();
	EFrameStatus EndFrame();

	void DestroyFrame();
	void DestroyBuffers();
	void DestroyCore();
	void DestroyInstance();

	void RecreateCore(const VkSurfaceKHR& _surface, int32_t _frameWidth, int32_t _frameHeight);
	void RecreateBuffers(int32_t _frameWidth, int32_t _frameHeight, uint32_t _layers /*= 1*/);

public:
	//////////////////////////////////////////////////////////////////////////
	// Generic functions and methods
	void WaitIdle();

	bool CreateRenderPass(int32_t _frameWidth, int32_t _frameHeight);
	bool CreateGraphicsPipeline(SharedPtr<VertexDescriptor> _vertexDescriptor, SharedPtr<Shader> _vertexShader, SharedPtr<Shader> _fragmentShader);

	bool RecordCommandBuffers(uint32_t _indexCount, uint32_t _instanceCount, uint32_t _firstIndex, uint32_t _vertexOffset, uint32_t _firstInstance);

public:
	//////////////////////////////////////////////////////////////////////////
	// Getters
	AK_INLINE SharedPtr<Instance> GetRenderInstance() const { return m_instance; }
	AK_INLINE SharedPtr<Device> GetDevice() const { return m_device; }

	// TODO: These will moved in the VertexCache when I'll made it!
	AK_INLINE SharedPtr<VertexBufferObject> GetVBO() const { return m_vbo; };
	AK_INLINE SharedPtr<IndexBufferObject> GetIBO() const { return m_ibo; };
	//

	AK_INLINE SharedPtr<SwapChain> GetSwapChain() const { return m_swapchain; }
	AK_INLINE SharedPtr<DescriptorSetLayout> GetDescriptorSetLayout() const { return m_descriptorSetLayout; }
	AK_INLINE SharedPtr<PipelineLayout> GetPipelineLayout() const { return m_pipelineLayout; }


private:
	RenderManager();
	~RenderManager();

	void InternalCreate(const VkSurfaceKHR& _surface, int32_t _frameWidth, int32_t _frameHeight);
	void InternalDetroy();

private:
	SharedPtr<Instance> m_instance;
	SharedPtr<Device> m_device;
	SharedPtr<SwapChain> m_swapchain;
	SharedPtr<CommandPool> m_commandPool;
	SharedPtr<RenderPass> m_renderPass;
	SharedPtr<PipelineCache> m_pipelineCache;
	SharedPtr<Pipeline> m_pipeline;
	SharedPtr<Frame> m_frame;
	SharedPtr<VertexBufferObject> m_vbo;
	SharedPtr<IndexBufferObject> m_ibo;
	SharedPtr<DescriptorSetLayout> m_descriptorSetLayout;
	SharedPtr<PipelineLayout> m_pipelineLayout;

	std::vector<SharedPtr<FrameBuffer>> m_frameBuffers;
	std::vector<SharedPtr<CommandBuffer>> m_commandBuffers;
};

AK_NAMESPACE_END