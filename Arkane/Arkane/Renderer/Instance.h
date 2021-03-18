#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Core/BasicDefines.h"


AK_NAMESPACE_BEGIN

class Instance final
{
public:
	Instance(const char* _appName, uint32_t _appVersion = VK_MAKE_VERSION(0, 0, 1));
	~Instance();

	AK_INLINE bool IsInitialized() const { return m_instance != VK_NULL_HANDLE; }
	AK_INLINE const VkInstance& GetInstance() const { return m_instance; }

private:
	void PopulateDebugReport(VkDebugReportCallbackCreateInfoEXT& _createInfo);
	void CreateDebugReport(const VkDebugReportCallbackCreateInfoEXT& _createInfo);
	void DestroyDebugReport();

private:
	VkInstance m_instance;
	VkDebugReportCallbackEXT m_debugCallback;
	bool m_validationLayerEnabled;
	bool m_debugReportCreated;
};

AK_NAMESPACE_END
