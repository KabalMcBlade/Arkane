#include "RenderManager.h"


#include "../Core/Assertions.h"
#include "../Core/VulkanAllocator.h"

#include "../Renderer/QueueFamily.h"
#include "../Renderer/Device.h"
#include "../Renderer/Instance.h"
#include "../Renderer/RenderPass.h"
#include "../Renderer/PipelineCache.h"
#include "../Renderer/Pipeline.h"
#include "../Renderer/FrameBuffer.h"
#include "../Renderer/CommandPool.h"
#include "../Renderer/StagingManager.h"
#include "../Renderer/CommandBuffer.h"
#include "../Renderer/Buffers/VertexBufferObject.h"
#include "../Renderer/Buffers/IndexBufferObject.h"
#include "../Renderer/Buffers/UniformBufferObject.h"
#include "../Renderer/DescriptorSetLayout.h"
#include "../Renderer/PipelineLayout.h"
#include "../Renderer/VertexDescriptor.h"
#include "../Renderer/DescriptorPool.h"
#include "../Renderer/DescriptorSet.h"
#include "../Renderer/VertexCache.h"

#include "../Shaders/Shader.h"

#include "../Utilities/Hash.h"



AK_NAMESPACE_BEGIN

RenderManager& RenderManager::GetInstance()
{
	static RenderManager instance;
	return instance;
}

void RenderManager::CreateInstance(const char* _appName, uint32_t _appVersion /*= VK_MAKE_VERSION(0, 0, 1)*/)
{
	m_instance = MakeSharedPtr<Instance>(_appName, _appVersion);
}

void RenderManager::CreateCore(const VkSurfaceKHR& _surface, int32_t _frameWidth, int32_t _frameHeight, VkPhysicalDeviceFeatures _enabledFeatures)
{
	m_device = MakeSharedPtr<Device>(m_instance->GetInstance(), _surface, _enabledFeatures);

	VulkanAllocator::GetInstance().CreateVMA(m_device);

	InternalCreate(_surface, _frameWidth, _frameHeight);

	StagingManager::GetInstance().Init(m_device, m_swapchain);
}

void RenderManager::CreateBuffers(int32_t _frameWidth, int32_t _frameHeight, uint32_t _layers /*= 1*/)
{
	const size_t count = m_swapchain->GetImageViewsCount();

	m_frameBuffers.resize(count);
	for (std::vector<SharedPtr<FrameBuffer>>::size_type i = 0; i < count; ++i)
	{
		m_frameBuffers[i] = MakeSharedPtr<FrameBuffer>(m_device, m_renderPass, _frameWidth, _frameHeight, _layers);
		m_frameBuffers[i]->PushAttachment(m_swapchain->GetImageView(i));
		m_frameBuffers[i]->Create();
	}

	m_commandBuffers.resize(count);
	for (std::vector<SharedPtr<CommandBuffer>>::size_type i = 0; i < count; ++i)
	{
		m_commandBuffers[i] = MakeSharedPtr<CommandBuffer>(m_device, m_commandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
	}
}

void RenderManager::CreateFrame()
{
	m_frame = MakeSharedPtr<Frame>(m_device, m_swapchain);
}

EFrameStatus RenderManager::BeginFrame()
{
	StagingManager::GetInstance().Flush();

	return m_frame->BeginDraw(m_swapchain);
}

EFrameStatus RenderManager::EndFrame()
{
	return m_frame->EndDraw(m_swapchain, m_commandBuffers);
}

void RenderManager::DestroyFrame()
{
	m_frame.reset();
}

void RenderManager::DestroyBuffers()
{
	for (std::vector<SharedPtr<CommandBuffer>>::size_type i = 0; i < m_commandBuffers.size(); ++i)
	{
		m_commandBuffers[i].reset();
	}
	m_commandBuffers.clear();

	for (std::vector<SharedPtr<FrameBuffer>>::size_type i = 0; i < m_frameBuffers.size(); ++i)
	{
		m_frameBuffers[i].reset();
	}
	m_frameBuffers.clear();
}

void RenderManager::DestroyCore()
{
	StagingManager::GetInstance().Shutdown();

	InternalDetroy();

	VulkanAllocator::GetInstance().DestroyVMA();

	m_device.reset();
}

void RenderManager::DestroyInstance()
{
	m_instance.reset();
}

void RenderManager::RecreateCore(const VkSurfaceKHR& _surface, int32_t _frameWidth, int32_t _frameHeight)
{
	DestroyFrame();
	DestroyBuffers();
	InternalDetroy();
	InternalCreate(_surface, _frameWidth, _frameHeight);
}

void RenderManager::RecreateBuffers(int32_t _frameWidth, int32_t _frameHeight, uint32_t _layers /*= 1*/)
{
	CreateBuffers(_frameWidth, _frameHeight, _layers);
	CreateFrame();
}

void RenderManager::InternalCreate(const VkSurfaceKHR& _surface, int32_t _frameWidth, int32_t _frameHeight)
{
	m_swapchain = MakeSharedPtr<SwapChain>(m_device, _surface, _frameWidth, _frameHeight);

	m_vertexCache = MakeSharedPtr<VertexCache>(m_device->GetPhysicalDeviceProperties().limits.minUniformBufferOffsetAlignment);

	m_commandPool = MakeSharedPtr<CommandPool>(m_device, (uint32_t)m_device->GetQueueFamily()->GetGraphicsFamily());

	m_renderPass = MakeSharedPtr<RenderPass>(m_device);
	m_pipelineCache = MakeSharedPtr<PipelineCache>(m_device);
	m_pipeline = MakeSharedPtr<Pipeline>(m_device);

	m_descriptorPool = MakeSharedPtr<DescriptorPool>(m_device);

	// TODO: These will moved in the VertexCache when I'll made it!
	m_vbo = MakeSharedPtr<VertexBufferObject>();
	m_ibo = MakeSharedPtr<IndexBufferObject>();

	const size_t count = m_swapchain->GetImageViewsCount();
	m_ubos.resize(count);
	for (std::vector<SharedPtr<UniformBufferObject>>::size_type i = 0; i < count; ++i)
	{
		m_ubos[i] = MakeSharedPtr<UniformBufferObject>();
	}
	//

	m_descriptorSetLayout = MakeSharedPtr<DescriptorSetLayout>(m_device);
	m_pipelineLayout = MakeSharedPtr<PipelineLayout>(m_device, m_descriptorSetLayout);

	m_descriptorSets.resize(count);
	for (std::vector<SharedPtr<DescriptorSet>>::size_type i = 0; i < count; ++i)
	{
		m_descriptorSets[i] = MakeSharedPtr<DescriptorSet>(m_device);
	}
}

void RenderManager::InternalDetroy()
{
	const size_t count = m_swapchain->GetImageViewsCount();

	m_descriptorSets.resize(count);
	for (std::vector<SharedPtr<DescriptorSet>>::size_type i = 0; i < count; ++i)
	{
		m_descriptorSets[i].reset();
	}
	m_descriptorSets.clear();

	m_pipelineLayout.reset();
	m_descriptorSetLayout.reset();


	// TODO: These will moved in the VertexCache when I'll made it!
	m_ubos.resize(count);
	for (std::vector<SharedPtr<UniformBufferObject>>::size_type i = 0; i < count; ++i)
	{
		m_ubos[i]->FreeBufferObject();
		m_ubos[i].reset();
	}
	m_ubos.clear();

	m_vbo->FreeBufferObject();
	m_vbo.reset();
	m_ibo->FreeBufferObject();
	m_ibo.reset();
	//

	m_descriptorPool.reset();

	m_pipeline.reset();
	m_pipelineCache.reset();
	m_renderPass.reset();

	m_commandPool.reset();

	m_vertexCache.reset();

	m_swapchain.reset();
}

void RenderManager::WaitIdle()
{
	vkDeviceWaitIdle(m_device->GetDevice());
}

bool RenderManager::CreateRenderPass(int32_t _frameWidth, int32_t _frameHeight)
{
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = m_swapchain->GetFormat();
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;

	VkSubpassDependency dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	m_renderPass->SetDimensions(_frameWidth, _frameHeight);
	m_renderPass->SetOffset(0, 0);
	m_renderPass->PushColorDepthStencilClearValue(0.0f, 0.0f, 0.0f, 1.0f);
	m_renderPass->PushAttachmentDescription(colorAttachment);
	m_renderPass->PushSubpassDependency(dependency);
	m_renderPass->PushSubpassDescription(subpass);
	//m_renderPass->SetFrameBuffer(m_framebuffer);		// NEVER CALL THIS FUNCTION HERE! CALL FROM CommandBuffer!

	return m_renderPass->Create();
}

bool RenderManager::CreateGraphicsPipeline(SharedPtr<VertexDescriptor> _vertexDescriptor, SharedPtr<Shader> _vertexShader, SharedPtr<Shader> _fragmentShader)
{
	m_pipeline->SetVertexInput(_vertexDescriptor);
	m_pipeline->SetTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
	m_pipeline->SetPolygonMode(VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE);
	m_pipeline->SetDepthStencil(VK_FALSE, VK_FALSE);
	m_pipeline->SetViewport(m_swapchain);
	m_pipeline->SetMultisample(VK_FALSE, VK_SAMPLE_COUNT_1_BIT);

	// Call Set without push will not add anything but will create the structure, which is need it
	//m_pipeline->PushDynamic();	
	m_pipeline->SetDynamics();

	m_pipeline->PushColorBlendAttachment(VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT, VK_FALSE);
	m_pipeline->SetColorBlendAttachments();

	m_pipeline->SetLayout(m_pipelineLayout);
	m_pipeline->SetRenderPass(m_renderPass);
	m_pipeline->SetCache(m_pipelineCache);

	m_pipeline->PushShader(_vertexShader);
	m_pipeline->PushShader(_fragmentShader);

	return m_pipeline->Create();
}

bool RenderManager::RecordCommandBuffers(uint32_t _indexCount, uint32_t _instanceCount, uint32_t _firstIndex, uint32_t _vertexOffset, uint32_t _firstInstance)
{
	for (size_t i = 0; i < m_commandBuffers.size(); i++)
	{
		if (m_commandBuffers[i]->Begin())
		{
			m_commandBuffers[i]->BeginRenderPass(m_renderPass, m_frameBuffers[i]->GetFrameBuffer());

			m_commandBuffers[i]->BindPipeline(m_pipeline);

			m_commandBuffers[i]->BindVertexBuffer(m_vbo, 0, 0);
			m_commandBuffers[i]->BindIndexBuffer(m_ibo, VK_INDEX_TYPE_UINT16, 0);

			m_commandBuffers[i]->BindDescriptorSet(m_pipelineLayout, m_descriptorSets[i]);
			
			m_commandBuffers[i]->DrawIndexed(_indexCount, _instanceCount, _firstIndex, _vertexOffset, _firstInstance);

			m_commandBuffers[i]->EndRenderPass();

			if (!m_commandBuffers[i]->End())
			{
				return false;
			}

			// TODO: Need to implement the vbo, ibo and ubo from vertex cache and using this! 
			//m_vertexCache->Update();
		}
	}

	return true;
}

RenderManager::RenderManager()
{
}

RenderManager::~RenderManager()
{
	DestroyFrame();
	DestroyBuffers();
}

AK_NAMESPACE_END