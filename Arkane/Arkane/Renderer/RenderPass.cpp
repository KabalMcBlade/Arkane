#include "RenderPass.h"

#include "Device.h"

#include "../Core/Assertions.h"
#include "../Core/VulkanAllocator.h"


AK_NAMESPACE_BEGIN


RenderPass::RenderPass(SharedPtr<Device> _device)
	: m_device(_device)
	, m_renderPass(VK_NULL_HANDLE)
	, m_framebuffer(VK_NULL_HANDLE)
	, m_width(0)
	, m_height(0)
	, m_offsetX(0)
	, m_offsetY(0)
{

}

RenderPass::~RenderPass()
{
	if (m_device != VK_NULL_HANDLE && m_renderPass != VK_NULL_HANDLE)
	{
		vkDestroyRenderPass(m_device->GetDevice(), m_renderPass, VulkanAllocator::GetInstance().GetCallbacks());
		m_renderPass = VK_NULL_HANDLE;
	}
}

bool RenderPass::Create()
{
	// not quite sure, but I would like the possibilities to make an empty render pass
	//nwAssertReturnValue(m_attachmentDescriptors.size() || m_subpassDescriptors.size() > 0 || m_subpassDependencies.size(), false, "Either one of attachment descriptor, subpass descriptor and subpass dependencies has to be filled");

	VkRenderPassCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	createInfo.attachmentCount = static_cast<uint32_t>(m_attachmentDescriptors.size());
	createInfo.pAttachments = m_attachmentDescriptors.data();
	createInfo.subpassCount = static_cast<uint32_t>(m_subpassDescriptors.size());
	createInfo.pSubpasses = m_subpassDescriptors.data();
	createInfo.dependencyCount = static_cast<uint32_t>(m_subpassDependencies.size());
	createInfo.pDependencies = m_subpassDependencies.data();

	VkResult result = vkCreateRenderPass(m_device->GetDevice(), &createInfo, VulkanAllocator::GetInstance().GetCallbacks(), &m_renderPass);
	akAssertReturnValue(result == VK_SUCCESS, false, "Cannot create RenderPass");

	return m_renderPass != VK_NULL_HANDLE;
}


const VkRenderPassBeginInfo RenderPass::GetBeginInfo() const
{
	VkRenderPassBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	beginInfo.renderPass = m_renderPass;
	beginInfo.renderArea.offset.x = m_offsetX;
	beginInfo.renderArea.offset.y = m_offsetY;
	beginInfo.framebuffer = m_framebuffer;
	beginInfo.renderArea.extent.width = m_width;
	beginInfo.renderArea.extent.height = m_height;
	beginInfo.clearValueCount = static_cast<uint32_t>(m_clearValues.size());
	beginInfo.pClearValues = m_clearValues.data();
	return beginInfo;
}

void RenderPass::SetDimensions(uint32_t _width, uint32_t _height)
{
	m_width = _width;
	m_height = _height;
}

void RenderPass::SetOffset(uint32_t _offsetX, uint32_t _offsetY)
{
	m_offsetX = _offsetX;
	m_offsetY = _offsetY;
}

void RenderPass::SetFrameBuffer(VkFramebuffer _framebuffer)
{
	m_framebuffer = _framebuffer;
}

void RenderPass::PushColorDepthStencilClearValue(float _r, float _g, float _b, float _a, float _depth /*= 0.0f*/, uint32_t _stencil /*= 0*/)
{
	VkClearValue clearValues{};
	clearValues.color.float32[0] = _r;
	clearValues.color.float32[1] = _g;
	clearValues.color.float32[2] = _b;
	clearValues.color.float32[3] = _a;
	clearValues.depthStencil.depth = _depth;
	clearValues.depthStencil.stencil = _stencil;
	m_clearValues.push_back(clearValues);
}

void RenderPass::PushAttachmentDescription(VkAttachmentDescription _description)
{
	m_attachmentDescriptors.push_back(_description);
}

void RenderPass::PushSubpassDependency(VkSubpassDependency _dependency)
{
	m_subpassDependencies.push_back(_dependency);
}

void RenderPass::PushSubpassDescription(VkSubpassDescription _description)
{
	m_subpassDescriptors.push_back(_description);
}

AK_NAMESPACE_END