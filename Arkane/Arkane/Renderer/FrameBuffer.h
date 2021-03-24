#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Core/Defines.h"
#include "../Core/SmartPointers.h"

#include <vector>


#pragma warning( disable : 4251 )	//export std

AK_NAMESPACE_BEGIN

class RenderPass;
class Device;
class AK_DLL FrameBuffer final
{
public:
	FrameBuffer(SharedPtr<Device> _device, SharedPtr<RenderPass> _renderPass, uint32_t _width, uint32_t _height, uint32_t _layers = 1);
	~FrameBuffer();

	bool Create(VkFramebufferCreateFlags _flags = 0);

	AK_INLINE const VkFramebuffer& GetFrameBuffer() const
	{
		return m_framebuffer;
	}

	void PushAttachment(const VkImageView& _attachment);

	AK_INLINE uint32_t GetAttachmentCount() const { return static_cast<uint32_t>(m_attachmentViews.size()); }
	AK_INLINE uint32_t GetWidth() const { return m_width; }
	AK_INLINE uint32_t GetHeight() const { return m_height; }
	AK_INLINE uint32_t GetLayerCount() const { return m_layers; }

private:
	SharedPtr<Device> m_device;
	SharedPtr<RenderPass> m_renderPass;

	VkFramebuffer m_framebuffer;
	uint32_t m_width;
	uint32_t m_height;
	uint32_t m_layers;

	std::vector<VkImageView> m_attachmentViews;
};

AK_NAMESPACE_END