#include "Pipeline.h"

#include "DescriptorSetLayout.h"
#include "Device.h"

#include "../Core/Assertions.h"
#include "../Core/VulkanAllocator.h"

#include "../Shaders/Shader.h"

#include "PipelineLayout.h"
#include "RenderPass.h"
#include "VertexDescriptor.h"
#include "PipelineCache.h"
#include "SwapChain.h"


AK_NAMESPACE_BEGIN

Pipeline::Pipeline(SharedPtr<Device> _device)
	: m_device(_device)
	//, m_pipeline(VK_NULL_HANDLE)
	, m_bindPoint(VK_PIPELINE_BIND_POINT_MAX_ENUM)
	//, m_layout(nullptr)
	//, m_renderPass(nullptr)
{
}

Pipeline::~Pipeline()
{
	if (m_device != VK_NULL_HANDLE && m_pipeline != VK_NULL_HANDLE)
	{
		vkDestroyPipeline(m_device->GetDevice(), m_pipeline, VulkanAllocator::Instance().GetCallbacks());
	}
}

bool Pipeline::Create(VkPipelineBindPoint _bindPoint /*= VK_PIPELINE_BIND_POINT_GRAPHICS*/)
{
	akAssertReturnValue(m_layout, false, "PipelineLayout is null!");
	akAssertReturnValue(m_renderPass, false, "RenderPass is null!");

	m_bindPoint = _bindPoint;

	VkGraphicsPipelineCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

	createInfo.stageCount = static_cast<uint32_t>(m_shaderStages.size());
	createInfo.pStages = m_shaderStages.data();
	createInfo.layout = m_layout->GetLayout();
	createInfo.renderPass = m_renderPass->GetRenderPass();

	createInfo.pVertexInputState = &m_vertexInput;
	createInfo.pInputAssemblyState = &m_inputAssembly;
	createInfo.pRasterizationState = &m_rasterization;
	createInfo.pColorBlendState = &m_colorBlend;
	createInfo.pMultisampleState = &m_multiSample;
	createInfo.pViewportState = &m_viewPort;
	createInfo.pDepthStencilState = &m_depthStencil;
	createInfo.pDynamicState = &m_dynamic;

	VkResult result = vkCreateGraphicsPipelines(m_device->GetDevice(), m_cache, 1, &createInfo, VulkanAllocator::Instance().GetCallbacks(), &m_pipeline);
	akAssertReturnValue(result == VK_SUCCESS, false, "Cannot generate Pipeline!");

	return m_pipeline != VK_NULL_HANDLE;
}


void Pipeline::SetVertexInput(SharedPtr<VertexDescriptor> _vertexDescriptor)
{
	m_vertexInput = _vertexDescriptor->GetVertexInput();
}

void Pipeline::SetTopology(VkPrimitiveTopology _topology /*= VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST*/, VkPipelineInputAssemblyStateCreateFlags _flags /*= 0*/, VkBool32 _primitiveRestartEnable /*= VK_FALSE*/)
{
	m_inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	m_inputAssembly.topology = _topology;
	m_inputAssembly.flags = _flags;
	m_inputAssembly.primitiveRestartEnable = _primitiveRestartEnable;
}

void Pipeline::SetPolygonMode(VkPolygonMode _polygonMode /*= VK_POLYGON_MODE_FILL*/, VkCullModeFlags _cullMode /*= VK_CULL_MODE_NONE*/, VkFrontFace _frontFace /*= VK_FRONT_FACE_CLOCKWISE*/, VkPipelineRasterizationStateCreateFlags _flags /*= 0*/)
{
	m_rasterization.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	m_rasterization.polygonMode = _polygonMode;
	m_rasterization.cullMode = _cullMode;
	m_rasterization.frontFace = _frontFace;
	m_rasterization.flags = _flags;
	m_rasterization.depthClampEnable = VK_FALSE;
	m_rasterization.lineWidth = 1.0f;
}

void Pipeline::SetDepthStencil(VkBool32 _depthTestEnabled /*= VK_TRUE*/, VkBool32 _depthWriteEnabled /*= VK_TRUE*/, VkCompareOp _depthOp /*= VK_COMPARE_OP_LESS_OR_EQUAL*/, VkCompareOp _stencilBackOp /*= VK_COMPARE_OP_ALWAYS*/, VkCompareOp _stencilFrontOp /*= VK_COMPARE_OP_MAX_ENUM*/)
{
	m_depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	m_depthStencil.depthTestEnable = _depthTestEnabled;
	m_depthStencil.depthWriteEnable = _depthWriteEnabled;
	m_depthStencil.depthCompareOp = _depthOp;
	m_depthStencil.back.compareOp = _stencilBackOp;
	if (_stencilFrontOp == VK_COMPARE_OP_MAX_ENUM)
	{
		m_depthStencil.front = m_depthStencil.back;
	}
	else
	{
		m_depthStencil.front.compareOp = _stencilFrontOp;
	}
}

void Pipeline::SetViewport(SharedPtr<SwapChain> _swapChain, float _viewportX /*= 0.0f*/, float _viewportY /*= 0.0f*/, int32_t _scissorOffsetX /*= 0*/, int32_t _scissorOffsetY /*= 0*/, VkPipelineViewportStateCreateFlags _flags /*= 0*/)
{	m_viewport.x = _viewportX;
	m_viewport.y = _viewportY;
	m_viewport.width = (float)_swapChain->GetExtent().width;
	m_viewport.height = (float)_swapChain->GetExtent().height;
	m_viewport.minDepth = 0.0f;
	m_viewport.maxDepth = 1.0f;

	m_scissor.offset.x = _scissorOffsetX;
	m_scissor.offset.y = _scissorOffsetY;
	m_scissor.extent = _swapChain->GetExtent();

	m_viewPort.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	m_viewPort.viewportCount = 1;
	m_viewPort.pViewports = &m_viewport;
	m_viewPort.scissorCount = 1;
	m_viewPort.pScissors = &m_scissor;
	m_viewPort.flags = _flags;
}

void Pipeline::SetMultisample(VkBool32 _enabled /*= VK_FALSE*/, VkSampleCountFlagBits _samples /*= VK_SAMPLE_COUNT_1_BIT*/, VkPipelineMultisampleStateCreateFlags _flags /*= 0*/)
{
	m_multiSample.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	m_multiSample.rasterizationSamples = _samples;
	m_multiSample.flags = _flags;
	m_multiSample.sampleShadingEnable = _enabled;
}

void Pipeline::PushDynamic(VkDynamicState _dynamicState)
{
	m_dynamicStates.push_back(_dynamicState);
}

void Pipeline::SetDynamics(VkPipelineDynamicStateCreateFlags _flags /*= 0*/)
{
	m_dynamic.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	m_dynamic.pDynamicStates = m_dynamicStates.data();
	m_dynamic.dynamicStateCount = static_cast<uint32_t>(m_dynamicStates.size());
	m_dynamic.flags = _flags;
}

void Pipeline::PushColorBlendAttachment(VkColorComponentFlags _colorWriteMask /*= 0xF*/, VkBool32 _blendEnable /*= VK_FALSE*/, VkBlendFactor _colorSourceBlend /*= VK_BLEND_FACTOR_ONE*/, VkBlendFactor _colorDestBlend /*= VK_BLEND_FACTOR_ZERO*/, VkBlendOp _colorOp /*= VK_BLEND_OP_ADD*/, VkBlendFactor _alphaSourceBlend /*= VK_BLEND_FACTOR_ONE*/, VkBlendFactor _alphaDestBlend /*= VK_BLEND_FACTOR_ZERO*/, VkBlendOp _alphaOp /*= VK_BLEND_OP_ADD*/)
{
	VkPipelineColorBlendAttachmentState colorBlend{};

	colorBlend.colorWriteMask = _colorWriteMask;
	colorBlend.blendEnable = _blendEnable;

	colorBlend.srcColorBlendFactor = _colorSourceBlend;
	colorBlend.dstColorBlendFactor = _colorSourceBlend;
	colorBlend.colorBlendOp = _colorOp;
	colorBlend.srcAlphaBlendFactor = _alphaSourceBlend;
	colorBlend.dstAlphaBlendFactor = _alphaDestBlend;
	colorBlend.alphaBlendOp = _alphaOp;

	m_colorBlendAttachment.push_back(colorBlend);
}

void Pipeline::SetColorBlendAttachments(VkLogicOp _logicOpIfEnabled /*= VK_LOGIC_OP_COPY*/, float _blendConstant0 /*= 0.0f*/, float _blendConstant1 /*= 0.0f*/, float _blendConstant2 /*= 0.0f*/, float _blendConstant3 /*= 0.0f*/)
{
	m_colorBlend.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	m_colorBlend.pAttachments = m_colorBlendAttachment.data();
	m_colorBlend.attachmentCount = static_cast<uint32_t>(m_colorBlendAttachment.size());

	bool isLogicOpEnable = false;
	for (std::vector<VkPipelineColorBlendAttachmentState>::size_type i = 0; i < m_colorBlendAttachment.size(); ++i)
	{
		isLogicOpEnable = m_colorBlendAttachment[i].blendEnable;
		if (isLogicOpEnable)
		{
			break;
		}
	}

	m_colorBlend.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	m_colorBlend.logicOpEnable = isLogicOpEnable;
	m_colorBlend.logicOp = _logicOpIfEnabled;
	m_colorBlend.blendConstants[0] = _blendConstant0;
	m_colorBlend.blendConstants[1] = _blendConstant1;
	m_colorBlend.blendConstants[2] = _blendConstant2;
	m_colorBlend.blendConstants[3] = _blendConstant3;
}


void Pipeline::SetLayout(SharedPtr<PipelineLayout> _layout)
{
	m_layout = _layout;
}

void Pipeline::SetRenderPass(SharedPtr<RenderPass> _renderPass)
{
	m_renderPass = _renderPass;
}


void Pipeline::SetCache(SharedPtr<PipelineCache> _cache)
{
	m_cache = _cache->GetPipelineCache();
}

void Pipeline::PushShader(SharedPtr<Shader> _shader)
{
	VkPipelineShaderStageCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	createInfo.stage = _shader->GetShaderStage();
	createInfo.pName = "main";
	createInfo.module = _shader->GetShaderModule();

	m_shaderStages.push_back(createInfo);
}

AK_NAMESPACE_END