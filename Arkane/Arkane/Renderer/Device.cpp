#include "Device.h"

#include "../Core/Assertions.h"
#include "../Core/VulkanAllocator.h"

#include <set>
#include <string>


#pragma warning(disable : 26812 26495)

AK_NAMESPACE_BEGIN


Device::Device(const VkInstance& _instance, const VkSurfaceKHR& _surface, const VkPhysicalDeviceFeatures& _enabledFeatures /*= {}*/)
	: m_enableValidationLayers(false)
	, m_instance(_instance)
	, m_surface(_surface)
	, m_enabledDeviceFeatures(_enabledFeatures)
	, m_device(VK_NULL_HANDLE)
	, m_queueFamily(nullptr)
	, m_physicalDevice(VK_NULL_HANDLE)
	, m_graphicsQueue(VK_NULL_HANDLE)
	, m_presentQueue(VK_NULL_HANDLE)
	, m_computeQueue(VK_NULL_HANDLE)
{
#if _DEBUG
	m_enableValidationLayers = true;
#else
	m_enableValidationLayers = false;
#endif

	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);

	akAssertReturnVoid(deviceCount > 0, "Failed to find GPU with supporting Vulkan API");

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data());

	for (VkPhysicalDevice device : devices)
	{
		if (IsDeviceSuitable(device))
		{
			m_physicalDevice = device;

			std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
			std::set<int32_t> uniqueQueueFamilies =
			{
				m_queueFamily->GetGraphicsFamily(),
				m_queueFamily->GetComputeFamily(),
				m_queueFamily->GetPresentFamily()
			};

			float queuePriority = 1.0f;
			for (int32_t queueFamily : uniqueQueueFamilies)
			{
				VkDeviceQueueCreateInfo queueCreateInfo = {};
				queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
				queueCreateInfo.queueFamilyIndex = queueFamily;
				queueCreateInfo.queueCount = 1;
				queueCreateInfo.pQueuePriorities = &queuePriority;
				queueCreateInfos.push_back(queueCreateInfo);
			}

			VkDeviceCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			createInfo.pQueueCreateInfos = queueCreateInfos.data();
			createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
			createInfo.pEnabledFeatures = &m_enabledDeviceFeatures;

			const char* const extensions[1] = { { VK_KHR_SWAPCHAIN_EXTENSION_NAME } };
			createInfo.enabledExtensionCount = static_cast<uint32_t>(1);
			createInfo.ppEnabledExtensionNames = extensions;

			if (m_enableValidationLayers)
			{
				const char* const validation[1] = { { "VK_LAYER_LUNARG_standard_validation" } };
				createInfo.enabledLayerCount = static_cast<uint32_t>(1);
				createInfo.ppEnabledLayerNames = validation;
			}
			else
			{
				createInfo.enabledLayerCount = 0;
			}

			VkResult result = vkCreateDevice(m_physicalDevice, &createInfo, VulkanAllocator::Instance().GetCallbacks(), &m_device);
			akAssertReturnVoid(result == VK_SUCCESS, "Cannot create logical device");

			vkGetDeviceQueue(m_device, m_queueFamily->GetGraphicsFamily(), 0, &m_graphicsQueue);
			vkGetDeviceQueue(m_device, m_queueFamily->GetComputeFamily(), 0, &m_presentQueue);
			vkGetDeviceQueue(m_device, m_queueFamily->GetPresentFamily(), 0, &m_computeQueue);

			vkGetPhysicalDeviceProperties(m_physicalDevice, &m_physicalDeviceProperties);
		}
	}
}

Device::~Device()
{
	if (m_queueFamily)
	{
		delete m_queueFamily;
	}

	if (m_device != VK_NULL_HANDLE)
	{
		vkDestroyDevice(m_device, VulkanAllocator::Instance().GetCallbacks());
		m_device = VK_NULL_HANDLE;
	}
}

bool Device::CheckDeviceExtensionSupport(VkPhysicalDevice _device, const std::vector<const char*>& _deviceExtensions)
{
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(_device, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(_device, nullptr, &extensionCount, availableExtensions.data());

	std::set<std::string> requiredExtensions(_deviceExtensions.begin(), _deviceExtensions.end());

	for (const VkExtensionProperties& extension : availableExtensions)
	{
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

bool Device::IsDeviceSuitable(VkPhysicalDevice _physicalDevice)
{
	if (m_queueFamily)
	{
		delete m_queueFamily;
	}

	m_queueFamily = new QueueFamily(_physicalDevice, m_surface);
	m_queueFamily->SetPhysicalDevice(_physicalDevice);
	m_queueFamily->FindQueueFamilies(VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT);

	bool extensionsSupported = CheckDeviceExtensionSupport(_physicalDevice, { VK_KHR_SWAPCHAIN_EXTENSION_NAME });

	bool swapChainAdequate = false;
	if (extensionsSupported)
	{
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_physicalDevice, m_surface, &capabilities);

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(_physicalDevice, m_surface, &formatCount, nullptr);

		if (formatCount != 0)
		{
			formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(_physicalDevice, m_surface, &formatCount, formats.data());
		}

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(_physicalDevice, m_surface, &presentModeCount, nullptr);

		if (presentModeCount != 0)
		{
			presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(_physicalDevice, m_surface, &presentModeCount, presentModes.data());
		}

		swapChainAdequate = !formats.empty() && !presentModes.empty();
	}

	bool isValidFamily = m_queueFamily->GetGraphicsFamily() >= 0 && m_queueFamily->GetPresentFamily() >= 0 && m_queueFamily->GetComputeFamily() >= 0;

	return isValidFamily && extensionsSupported && swapChainAdequate;
}

AK_NAMESPACE_END