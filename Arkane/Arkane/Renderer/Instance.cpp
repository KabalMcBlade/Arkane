#include "Instance.h"

#include "../Core/Assertions.h"
#include "../Core/VulkanAllocator.h"

#include <string>
#include <vector>
#include <iostream>

#pragma warning(disable : 26812 26495)

AK_NAMESPACE_BEGIN

#define NW_VALIDATION_LAYER "VK_LAYER_KHRONOS_validation"

//////////////////////////////////////////////////////////////////////////

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType, uint64_t obj, size_t location, int32_t code, const char* layerPrefix, const char* msg, void* userData)
{
	std::string prefix;
	if (flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT)
	{
		prefix = "[INFO]";
	};
	if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT)
	{
		prefix = "[WARNING]";
	};
	if (flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT)
	{
		prefix = "[PERFORMANCE]";
	};
	if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
	{
		prefix = "[ERROR]";
	};
	if (flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT)
	{
		prefix = "[DEBUG]";
	}

	std::cerr << prefix << "[TYPE:" << layerPrefix << "]" << std::endl << msg << std::endl << std::endl;
	return VK_FALSE;
}

//////////////////////////////////////////////////////////////////////////

Instance::Instance(const char* _appName, uint32_t _appVersion /*= VK_MAKE_VERSION(0, 0, 1)*/) : m_instance(VK_NULL_HANDLE), m_debugReportCreated(false)
{
#if _DEBUG
	m_validationLayerEnabled = true;
#else
	m_validationLayerEnabled = false;
#endif

	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = _appName;
	appInfo.pEngineName = "Arkane";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_2;
	appInfo.applicationVersion = _appVersion;

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.pApplicationInfo = &appInfo;

	std::vector<const char*> enabledExtensions;
	enabledExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
	enabledExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);

	VkDebugReportCallbackCreateInfoEXT validationCreateInfo = {};

	std::vector<const char*> enabledLayers;
	if (m_validationLayerEnabled && !m_debugReportCreated)
	{
		enabledExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);

		enabledLayers.push_back(NW_VALIDATION_LAYER);

		uint32_t layerCount = 0;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> layers;
		layers.resize(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, layers.data());

		bool found = false;
		for (uint32_t i = 0; i < layerCount; ++i)
		{
			if (std::strcmp(NW_VALIDATION_LAYER, layers[i].layerName) == 0)
			{
				found = true;
				break;
			}
		}
		akAssertReturnVoid(found, "Cannot find validation layer");

		PopulateDebugReport(validationCreateInfo);
		createInfo.pNext = (VkDebugReportCallbackCreateInfoEXT*)&validationCreateInfo;

		createInfo.enabledLayerCount = static_cast<uint32_t>(enabledLayers.size());
		createInfo.ppEnabledLayerNames = enabledLayers.data();
	}

	createInfo.enabledExtensionCount = static_cast<uint32_t>(enabledExtensions.size());
	createInfo.ppEnabledExtensionNames = enabledExtensions.data();

	VkResult result = vkCreateInstance(&createInfo, VulkanAllocator::Instance().GetCallbacks(), &m_instance);

	if (m_validationLayerEnabled && !m_debugReportCreated && result == VK_SUCCESS)
	{
		CreateDebugReport(validationCreateInfo);

		m_debugReportCreated = true;
	}

	akAssertReturnVoid(result == VK_SUCCESS, "Cannot create instance");
}

Instance::~Instance()
{
	if (m_validationLayerEnabled)
	{
		DestroyDebugReport();
	}

	if (m_instance != VK_NULL_HANDLE)
	{
		vkDestroyInstance(m_instance, VulkanAllocator::Instance().GetCallbacks());
		m_instance = VK_NULL_HANDLE;
	}
}

void Instance::PopulateDebugReport(VkDebugReportCallbackCreateInfoEXT& createInfo)
{
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
	createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT | VK_DEBUG_REPORT_DEBUG_BIT_EXT;
	createInfo.pfnCallback = DebugCallback;
}

void Instance::CreateDebugReport(const VkDebugReportCallbackCreateInfoEXT& createInfo)
{
	auto func = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(m_instance, "vkCreateDebugReportCallbackEXT");
	if (func != nullptr)
	{
		VkResult result = func(m_instance, &createInfo, VulkanAllocator::Instance().GetCallbacks(), &m_debugCallback);
		akAssertReturnVoid(result == VK_SUCCESS, "Impossible create Debug Report!");
	}
}

void Instance::DestroyDebugReport()
{
	auto func = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(m_instance, "vkDestroyDebugReportCallbackEXT");
	if (func != nullptr)
	{
		func(m_instance, m_debugCallback, VulkanAllocator::Instance().GetCallbacks());
	}
}

AK_NAMESPACE_END