#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Core/Defines.h"
#include "../Core/SmartPointers.h"

#include "QueueFamily.h"

#include <memory>
#include <vector>


#pragma warning(disable:4251)

AK_NAMESPACE_BEGIN

class Device;

class AK_DLL SwapChain final
{
public:
	struct SwapChainDetails
	{
		VkSurfaceCapabilitiesKHR m_capabilities;
		std::vector<VkSurfaceFormatKHR> m_formats;
		std::vector<VkPresentModeKHR> m_presentModes;
	};

	SwapChain::SwapChainDetails QuerySwapChainSupport(const VkPhysicalDevice& _device, VkSurfaceKHR _surface);

public:
	SwapChain(SharedPtr<Device> _device, const VkSurfaceKHR& _surface, uint32_t _width, uint32_t _height);
	~SwapChain();

	bool RecreateSwapChain();

	AK_INLINE VkSwapchainKHR GetSwapChain() const { return m_swapChain; }

	AK_INLINE const std::vector<VkImageView>& GetImageViews() const { return m_swapChainImageViews; }
	AK_INLINE const VkImageView& GetImageView(size_t _index) const { return m_swapChainImageViews[_index]; }
	AK_INLINE size_t GetImageViewsCount() const { return m_swapChainImageViews.size(); }

	AK_INLINE VkExtent2D GetExtent() const { return m_swapChainExtent; }
	AK_INLINE VkFormat GetFormat() const { return m_swapChainImageFormat; }
	AK_INLINE uint32_t GetSwapImageSize() const { return static_cast<uint32_t>(m_swapChainImages.size()); }

private:
	bool CreateSwapChain();
	bool CreateSwapChainImageViews();

	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& _availableFormats);
	VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& _availablePresentModes);
	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& _capabilities);
	
private:
	SharedPtr<Device> m_device;
	VkSurfaceKHR m_surface;
	uint32_t m_width;
	uint32_t m_height;

	std::vector<VkImage> m_swapChainImages;
	std::vector<VkImageView> m_swapChainImageViews;
	VkSwapchainKHR m_swapChain;
	VkFormat m_swapChainImageFormat;
	VkExtent2D m_swapChainExtent;
};

AK_NAMESPACE_END