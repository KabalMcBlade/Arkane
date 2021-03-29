#include "SwapChain.h"

#include <algorithm>

#include "Device.h"

#include "../Core/Assertions.h"
#include "../Core/VulkanAllocator.h"

#include "Frame.h"	// just for frames of flights static value


AK_NAMESPACE_BEGIN

SwapChain::SwapChainDetails SwapChain::QuerySwapChainSupport(const VkPhysicalDevice& _device, VkSurfaceKHR _surface)
{
	SwapChain::SwapChainDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_device, _surface, &details.m_capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(_device, _surface, &formatCount, nullptr);

	if (formatCount != 0)
	{
		details.m_formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(_device, _surface, &formatCount, details.m_formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(_device, _surface, &presentModeCount, nullptr);

	if (presentModeCount != 0)
	{
		details.m_presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(_device, _surface, &presentModeCount, details.m_presentModes.data());
	}

	return details;
}

//////////////////////////////////////////////////////////////////////////

SwapChain::SwapChain(SharedPtr<Device> _device, const VkSurfaceKHR& _surface, uint32_t _width, uint32_t _height) : m_device(_device), m_surface(_surface), m_swapChain(VK_NULL_HANDLE), m_width(_width), m_height(_height)
{
	RecreateSwapChain();
}

SwapChain::~SwapChain()
{
	if (m_device->GetDevice() != VK_NULL_HANDLE)
	{
		if (m_swapChain != VK_NULL_HANDLE)
		{
			for (uint32_t i = 0; i < m_swapChainImageViews.size(); ++i)
			{
				vkDestroyImageView(m_device->GetDevice(), m_swapChainImageViews[i], VulkanAllocator::Instance().GetCallbacks());
			}

			m_swapChainImageViews.clear();

			vkDestroySwapchainKHR(m_device->GetDevice(), m_swapChain, VulkanAllocator::Instance().GetCallbacks());
		}
		m_swapChain = VK_NULL_HANDLE;
	}
}

bool SwapChain::RecreateSwapChain()
{
	if (!CreateSwapChain())
	{
		return false;
	}

	if (!CreateSwapChainImageViews())
	{
		return false;
	}

	return true;
}

bool SwapChain::CreateSwapChain()
{
	SwapChain::SwapChainDetails swapChainSupport = SwapChain::QuerySwapChainSupport(m_device->GetPhysicalDevice(), m_surface);

	VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.m_formats);
	VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.m_presentModes);
	VkExtent2D extent = ChooseSwapExtent(swapChainSupport.m_capabilities);

	uint32_t imageCount = swapChainSupport.m_capabilities.minImageCount + 1;
	if (swapChainSupport.m_capabilities.maxImageCount > 0 && imageCount > swapChainSupport.m_capabilities.maxImageCount)
	{
		imageCount = swapChainSupport.m_capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = m_surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1; // 2 for VR or generic stereoscopic 
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	SharedPtr<QueueFamily> indices = m_device->GetQueueFamily();
	uint32_t queueFamilyIndices[] = { (uint32_t)indices->GetGraphicsFamily(), (uint32_t)indices->GetPresentFamily() };

	if (indices->GetGraphicsFamily() != indices->GetPresentFamily())
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = nullptr;
	}

	createInfo.preTransform = swapChainSupport.m_capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;

	VkSwapchainKHR oldSwapChain = m_swapChain;
	createInfo.oldSwapchain = oldSwapChain;

	VkSwapchainKHR newSwapChain;
	VkResult result = vkCreateSwapchainKHR(m_device->GetDevice(), &createInfo, VulkanAllocator::Instance().GetCallbacks(), &newSwapChain);
	akAssertReturnValue(result == VK_SUCCESS, false, "Failed to create swap chain");

	m_swapChain = newSwapChain;

	vkGetSwapchainImagesKHR(m_device->GetDevice(), m_swapChain, &imageCount, nullptr);
	m_swapChainImages.resize(imageCount);

	vkGetSwapchainImagesKHR(m_device->GetDevice(), m_swapChain, &imageCount, m_swapChainImages.data());
	m_swapChainImageFormat = surfaceFormat.format;
	m_swapChainExtent = extent;

	return true;
}

bool SwapChain::CreateSwapChainImageViews()
{
	m_swapChainImageViews.resize(m_swapChainImages.size(), VK_NULL_HANDLE);

	for (uint32_t i = 0; i < m_swapChainImages.size(); i++)
	{
		VkImageViewCreateInfo viewInfo = {};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = m_swapChainImages[i];
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = m_swapChainImageFormat;
		viewInfo.components = {};
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;
		viewInfo.flags = 0;

		VkResult result = vkCreateImageView(m_device->GetDevice(), &viewInfo, VulkanAllocator::Instance().GetCallbacks(), &m_swapChainImageViews[i]);
		akAssertReturnValue(result == VK_SUCCESS, false, "Cannot get swap chain image");
	}

	Frame::kNumOfFramesInFlight = m_swapChainImages.size();

	return true;
}

VkSurfaceFormatKHR SwapChain::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& _availableFormats)
{
	if (_availableFormats.size() == 1 && _availableFormats[0].format == VK_FORMAT_UNDEFINED)
	{
		return{ VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
	}

	for (const VkSurfaceFormatKHR& availableFormat : _availableFormats)
	{
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			return availableFormat;
		}
	}

	return _availableFormats[0];
}

VkPresentModeKHR SwapChain::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& _availablePresentModes)
{
	for (const VkPresentModeKHR& availablePresentMode : _availablePresentModes)
	{
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			return availablePresentMode;
		}
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D SwapChain::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
	{
		return capabilities.currentExtent;
	}
	else
	{
		VkExtent2D actualExtent = { m_width, m_height };

		actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}
}

AK_NAMESPACE_END