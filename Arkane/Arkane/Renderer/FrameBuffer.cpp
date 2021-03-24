#include "FrameBuffer.h"

#include "../Core/Assertions.h"
#include "../Core/VulkanAllocator.h"

#include "Device.h"
#include "RenderPass.h"


AK_NAMESPACE_BEGIN

FrameBuffer::FrameBuffer(SharedPtr<Device> _device, SharedPtr<RenderPass> _renderPass, uint32_t _width, uint32_t _height, uint32_t _layers /*= 1*/)
	: m_device(_device)
	, m_renderPass(_renderPass)
	, m_framebuffer(VK_NULL_HANDLE)
	, m_width(_width)
	, m_height(_height)
	, m_layers(_layers)
{

}

FrameBuffer::~FrameBuffer()
{
	if (m_device != VK_NULL_HANDLE && m_framebuffer != VK_NULL_HANDLE)
	{
		vkDestroyFramebuffer(m_device->GetDevice(), m_framebuffer, VulkanAllocator::Instance().GetCallbacks());
		m_framebuffer = VK_NULL_HANDLE;
	}
}

bool FrameBuffer::Create(VkFramebufferCreateFlags _flags/* = 0*/)
{
	VkFramebufferCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	info.renderPass = m_renderPass->GetRenderPass();
	info.attachmentCount = static_cast<uint32_t>(m_attachmentViews.size());
	info.pAttachments = m_attachmentViews.data();
	info.width = m_width;
	info.height = m_height;
	info.layers = m_layers;
	info.flags = _flags;

	VkResult result = vkCreateFramebuffer(m_device->GetDevice(), &info, VulkanAllocator::Instance().GetCallbacks(), &m_framebuffer);
	akAssertReturnValue(result == VK_SUCCESS, false, "Cannot create FrameBuffer");

	return m_framebuffer != VK_NULL_HANDLE;
}

void FrameBuffer::PushAttachment(const VkImageView& _attachment)
{
	m_attachmentViews.push_back(_attachment);
}

AK_NAMESPACE_END