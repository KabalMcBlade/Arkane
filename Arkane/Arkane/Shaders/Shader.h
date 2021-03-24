#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Core/Defines.h"
#include "../Core/SmartPointers.h"

#include "../Renderer/Device.h"

#include <string>


#pragma warning(disable:4251)

AK_NAMESPACE_BEGIN

class FileSystem;
class AK_DLL Shader final
{
public:
	Shader(SharedPtr<Device> _device, const std::string& _path);
	~Shader();

	AK_INLINE const VkShaderModule& GetShaderModule() const { return m_module; }
	AK_INLINE VkShaderModule& GetShaderModule() { return m_module; }

	AK_INLINE const VkShaderStageFlagBits& GetShaderStage() const { return m_stage; }
	AK_INLINE VkShaderStageFlagBits& GetShaderStage() { return m_stage; }

	AK_INLINE const uint32_t GetId() const { return m_hash; }

private:
	friend class ShaderManager;

	static VkShaderStageFlagBits ConvertExtToShaderStage(const std::string& _ext);

private:
	SharedPtr<Device> m_device;
	VkShaderModule m_module;
	VkShaderStageFlagBits m_stage;
	uint32_t m_hash;
};

AK_NAMESPACE_END
