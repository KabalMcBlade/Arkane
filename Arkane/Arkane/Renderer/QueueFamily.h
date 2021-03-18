#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Core/Defines.h"


AK_NAMESPACE_BEGIN

class QueueFamily final
{
public:
	QueueFamily(VkPhysicalDevice _physicalDevice, VkSurfaceKHR _surface);
	~QueueFamily();

	void FindQueueFamilies(VkQueueFlags _desiredFamilies = VK_QUEUE_GRAPHICS_BIT);
	void Clear();
	void SetPhysicalDevice(VkPhysicalDevice _physicslDevice);

	int32_t GetGraphicsFamily() const { return m_graphicsFamily; }
	int32_t GetPresentFamily() const { return m_presentFamily; }
	int32_t GetComputeFamily() const { return m_computeFamily; }
	int32_t GetTransferFamily() const { return m_transferFamily; }

private:
	VkPhysicalDevice m_physicalDevice;
	VkSurfaceKHR m_surface;

	int32_t m_graphicsFamily = -1;
	int32_t m_presentFamily = -1;
	int32_t m_computeFamily = -1;
	int32_t m_transferFamily = -1;
};

AK_NAMESPACE_END