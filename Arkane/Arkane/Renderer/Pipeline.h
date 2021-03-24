#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Core/Defines.h"
#include "../Core/SmartPointers.h"

#include <vector>


#pragma warning( disable : 4251 )	//export std

AK_NAMESPACE_BEGIN

class RenderPass;
class Shader;
class PipelineLayout;
class VertexDescriptor;
class Device;
class PipelineCache;
class SwapChain;
class AK_DLL Pipeline final
{
public:
	Pipeline(SharedPtr<Device> _device);
	~Pipeline();

	AK_INLINE const VkPipeline& GetPipeline() const { return m_pipeline; }
	AK_INLINE const VkPipelineBindPoint& GetPipelineBindingPoint() const { return m_bindPoint; }
	AK_INLINE const VkViewport& GetViewport() const { return m_viewport; }
	AK_INLINE const VkRect2D& GetScissor() const { return m_scissor; }

	bool Create(VkPipelineBindPoint _bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS);

	// --
	// Need to call all of them explicitly!
	void SetVertexInput(SharedPtr<VertexDescriptor> _vertexDescriptor);

	void SetTopology(VkPrimitiveTopology _topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VkPipelineInputAssemblyStateCreateFlags _flags = 0, VkBool32 _primitiveRestartEnable = VK_FALSE);

	void SetPolygonMode(VkPolygonMode _polygonMode = VK_POLYGON_MODE_FILL, VkCullModeFlags _cullMode = VK_CULL_MODE_NONE, VkFrontFace _frontFace = VK_FRONT_FACE_CLOCKWISE, VkPipelineRasterizationStateCreateFlags _flags = 0);

	// if stencil op front is VK_COMPARE_OP_MAX_ENUM, it simple sets the front equal to the back
	void SetDepthStencil(VkBool32 _depthTestEnabled = VK_TRUE, VkBool32 _depthWriteEnabled = VK_TRUE, VkCompareOp _depthOp = VK_COMPARE_OP_LESS_OR_EQUAL, VkCompareOp _stencilBackOp = VK_COMPARE_OP_ALWAYS, VkCompareOp _stencilFrontOp = VK_COMPARE_OP_MAX_ENUM);

	void SetViewport(SharedPtr<SwapChain> _swapChain, float _viewportX = 0.0f, float _viewportY = 0.0f, int32_t _scissorOffsetX = 0, int32_t _scissorOffsetY = 0, VkPipelineViewportStateCreateFlags _flags = 0);

	void SetMultisample(VkBool32 _enabled = VK_FALSE, VkSampleCountFlagBits _samples = VK_SAMPLE_COUNT_1_BIT, VkPipelineMultisampleStateCreateFlags _flags = 0);

	// dynamic state combo functions
	void PushDynamic(VkDynamicState _dynamicState);
	void SetDynamics(VkPipelineDynamicStateCreateFlags _flags = 0);

	// color blend attachments state combo functions
	// must be 1 per pass
	void PushColorBlendAttachment(VkColorComponentFlags _colorWriteMask = 0xF, VkBool32 _blendEnable = VK_FALSE, VkBlendFactor _colorSourceBlend = VK_BLEND_FACTOR_ONE, VkBlendFactor _colorDestBlend = VK_BLEND_FACTOR_ZERO, VkBlendOp _colorOp = VK_BLEND_OP_ADD, VkBlendFactor _alphaSourceBlend = VK_BLEND_FACTOR_ONE, VkBlendFactor _alphaDestBlend = VK_BLEND_FACTOR_ZERO, VkBlendOp _alphaOp = VK_BLEND_OP_ADD);
	void SetColorBlendAttachments(VkLogicOp _logicOpIfEnabled = VK_LOGIC_OP_COPY, float _blendConstant0 = 0.0f, float _blendConstant1 = 0.0f, float _blendConstant2 = 0.0f, float _blendConstant3 = 0.0f);
	// --

	void SetLayout(SharedPtr<PipelineLayout> _layout);
	void SetRenderPass(SharedPtr<RenderPass> _renderPass);
	void SetCache(SharedPtr<PipelineCache> _cache);

	void PushShader(SharedPtr<Shader> _shader);

private:
	SharedPtr<Device> m_device;
	SharedPtr<PipelineLayout> m_layout;
	SharedPtr<RenderPass> m_renderPass;

	VkPipeline m_pipeline{};
	VkPipelineBindPoint m_bindPoint{};
	VkPipelineCache m_cache{};

	VkViewport m_viewport{};
	VkRect2D m_scissor{};

	VkPipelineVertexInputStateCreateInfo m_vertexInput{};
	VkPipelineInputAssemblyStateCreateInfo m_inputAssembly{};
	VkPipelineRasterizationStateCreateInfo m_rasterization{};
	VkPipelineDepthStencilStateCreateInfo m_depthStencil{};
	VkPipelineViewportStateCreateInfo m_viewPort{};
	VkPipelineMultisampleStateCreateInfo m_multiSample{};
	VkPipelineDynamicStateCreateInfo m_dynamic{};
	VkPipelineColorBlendStateCreateInfo m_colorBlend{};

	std::vector<VkDynamicState> m_dynamicStates;
	std::vector<VkPipelineColorBlendAttachmentState> m_colorBlendAttachment;
	std::vector<VkPipelineShaderStageCreateInfo> m_shaderStages;
};

AK_NAMESPACE_END