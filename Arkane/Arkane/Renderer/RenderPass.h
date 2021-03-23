#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "VertexLayout.h"

#include "../Core/Defines.h"

#include <vector>


#pragma warning( disable : 4251 )	//export std


AK_NAMESPACE_BEGIN

class Device;
class AK_DLL RenderPass final
{
public:
	RenderPass(const Device& _device);
	~RenderPass();

	bool Create();

	AK_INLINE const VkRenderPass& GetRenderPass() const
	{
		return m_renderPass;
	}

	AK_INLINE const VkFramebuffer& GetFramebuffer() const
	{
		return m_framebuffer;
	}

	//////////////////////////////////////////////////////////////////////////
	AK_INLINE const size_t GetAttachmentDescriptorsCount() const
	{
		return m_attachmentDescriptors.size();
	}

	AK_INLINE const VkAttachmentDescription* GetAttachmentDescriptors() const
	{
		return m_attachmentDescriptors.data();
	}

	AK_INLINE const size_t GetSubpassDependenciesCount() const
	{
		return m_subpassDependencies.size();
	}

	AK_INLINE const VkSubpassDependency* GetSubpassDependencies() const
	{
		return m_subpassDependencies.data();
	}

	AK_INLINE const size_t GetSubpassDescriptorsCount() const
	{
		return m_subpassDescriptors.size();
	}

	AK_INLINE const VkSubpassDescription* GetSubpassDescriptors() const
	{
		return m_subpassDescriptors.data();
	}

	AK_INLINE const size_t GetClearValuesCount() const
	{
		return m_clearValues.size();
	}

	AK_INLINE const VkClearValue* GetClearValues() const
	{
		return m_clearValues.data();
	}
	//////////////////////////////////////////////////////////////////////////

	const VkRenderPassBeginInfo GetBeginInfo() const;

	void SetDimensions(uint32_t _width, uint32_t _height);
	void SetOffset(uint32_t _offsetX, uint32_t _offsetY);
	void SetFrameBuffer(VkFramebuffer _framebuffer);

	void PushColorDepthStencilClearValue(float _r, float _g, float _b, float _a, float _depth = 0.0f, uint32_t _stencil = 0);
	void PushAttachmentDescription(VkAttachmentDescription _description);
	void PushSubpassDependency(VkSubpassDependency _dependency);
	void PushSubpassDescription(VkSubpassDescription _description);

private:
	VkDevice m_device;
	VkRenderPass m_renderPass;
	VkFramebuffer m_framebuffer;
	uint32_t m_width;
	uint32_t m_height;
	uint32_t m_offsetX;
	uint32_t m_offsetY;
	std::vector<VkAttachmentDescription> m_attachmentDescriptors;
	std::vector<VkSubpassDependency> m_subpassDependencies;
	std::vector<VkSubpassDescription> m_subpassDescriptors;
	std::vector<VkClearValue> m_clearValues;
};

AK_NAMESPACE_END