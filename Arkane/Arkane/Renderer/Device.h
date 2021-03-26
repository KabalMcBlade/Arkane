#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Core/Defines.h"
#include "../Core/SmartPointers.h"

#include "QueueFamily.h"

#include <memory>
#include <vector>

AK_NAMESPACE_BEGIN

class Device final
{
public:
	Device(const VkInstance& _instance, const VkSurfaceKHR& _surface, const VkPhysicalDeviceFeatures& _enabledFeatures = {});
	~Device();

	AK_INLINE const VkPhysicalDevice& GetPhysicalDevice() const
	{
		return m_physicalDevice;
	}

	AK_INLINE const VkDevice& GetDevice() const
	{
		return m_device;
	}

	AK_INLINE const VkInstance& GetInstance() const
	{
		return m_instance;
	}

	AK_INLINE SharedPtr<QueueFamily> GetQueueFamily() const
	{
		return m_queueFamily;
	}

	AK_INLINE const VkPhysicalDeviceProperties& GetPhysicalDeviceProperties() const
	{
		return m_physicalDeviceProperties;
	}

	VkQueue GetGraphicsQueue() const { return m_graphicsQueue; }
	VkQueue GetComputeQueue() const { return m_computeQueue; }
	VkQueue GetPresentQueue() const { return m_presentQueue; }

private:
	bool CheckDeviceExtensionSupport(VkPhysicalDevice _device, const std::vector<const char *>& _deviceExtensions);
	bool IsDeviceSuitable(VkPhysicalDevice _physicalDevice);
	uint32_t RateDeviceSuitability(VkPhysicalDevice _physicalDevice);

private:
	VkInstance m_instance;
	VkSurfaceKHR m_surface;
	VkPhysicalDeviceFeatures m_enabledDeviceFeatures;

	VkPhysicalDeviceProperties m_physicalDeviceProperties;
	VkPhysicalDevice m_physicalDevice;
	VkDevice m_device;

	SharedPtr<QueueFamily> m_queueFamily;

	VkQueue m_graphicsQueue;
	VkQueue m_presentQueue;
	VkQueue m_computeQueue;

	bool m_enableValidationLayers;
};

AK_NAMESPACE_END