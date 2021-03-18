#include "QueueFamily.h"

#include "../Core/Assertions.h"

#include <vector>


AK_NAMESPACE_BEGIN

QueueFamily::QueueFamily(VkPhysicalDevice _physicalDevice, VkSurfaceKHR _surface)
	: m_physicalDevice(_physicalDevice)
	, m_surface(_surface)
{
	Clear();
}

QueueFamily::~QueueFamily()
{
	Clear();
}

void QueueFamily::FindQueueFamilies(VkQueueFlags _desiredFamilies /*= VK_QUEUE_GRAPHICS_BIT*/)
{
	bool wantGraphics = _desiredFamilies & VK_QUEUE_GRAPHICS_BIT;
	bool dedicatedCompute = _desiredFamilies & VK_QUEUE_COMPUTE_BIT;
	bool dedicatedTransfer = _desiredFamilies & VK_QUEUE_TRANSFER_BIT;

	VkPhysicalDevice device = m_physicalDevice;
	VkSurfaceKHR surface = m_surface;

	Clear();

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	if (wantGraphics)
	{
		for (uint32_t i = 0; i < queueFamilyCount; ++i)
		{
			const VkQueueFamilyProperties& queueFamily = queueFamilies[i];

			if (queueFamily.queueCount > 0 && m_graphicsFamily < 0 && (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT))
			{
				m_graphicsFamily = i;
				break;
			}
		}

		for (uint32_t i = 0; i < queueFamilyCount; ++i)
		{
			const VkQueueFamilyProperties& queueFamily = queueFamilies[i];

			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
			if (queueFamily.queueCount > 0 && m_presentFamily < 0 && presentSupport)
			{
				m_presentFamily = i;
				break;
			}
		}
	}

	if (dedicatedCompute)
	{
		for (uint32_t i = 0; i < queueFamilies.size(); ++i)
		{
			const VkQueueFamilyProperties& queueFamily = queueFamilies[i];

			if (queueFamily.queueCount > 0 && m_computeFamily < 0 && i != m_graphicsFamily && i != m_presentFamily && (queueFamily.queueCount & VK_QUEUE_COMPUTE_BIT))
			{
				m_computeFamily = i;
				break;
			}
		}

		if (m_computeFamily < 0)
		{
			m_computeFamily = m_graphicsFamily;
		}
	}
	else
	{
		m_computeFamily = m_graphicsFamily;
	}

	if (dedicatedTransfer)
	{
		for (uint32_t i = 0; i < queueFamilies.size(); ++i)
		{
			const VkQueueFamilyProperties& queueFamily = queueFamilies[i];

			if (queueFamily.queueCount > 0 && m_transferFamily < 0 && i != m_graphicsFamily && i != m_presentFamily && i != m_computeFamily && (queueFamily.queueCount & VK_QUEUE_TRANSFER_BIT))
			{
				m_transferFamily = i;
				break;
			}
		}

		if (m_transferFamily < 0)
		{
			m_transferFamily = m_graphicsFamily < 0 ? m_computeFamily : m_graphicsFamily;
		}
	}
	else
	{
		m_transferFamily = m_graphicsFamily < 0 ? m_computeFamily : m_graphicsFamily;
	}

	akAssertReturnVoid(wantGraphics && (m_graphicsFamily >= 0 && m_presentFamily >= 0), "FindQueueFamilies: no suitable graphics or present queue were found.");
	akAssertReturnVoid(wantGraphics && m_computeFamily >= 0, "FindQueueFamilies: cannot find compute queue.");
	akAssertReturnVoid(wantGraphics && m_transferFamily >= 0, "FindQueueFamilies: cannot find transfer queue.");
}

void QueueFamily::Clear()
{
	m_graphicsFamily = -1;
	m_presentFamily = -1;
	m_computeFamily = -1;
	m_transferFamily = -1;
}

void QueueFamily::SetPhysicalDevice(VkPhysicalDevice _physicslDevice)
{
	m_physicalDevice = _physicslDevice;
}

AK_NAMESPACE_END