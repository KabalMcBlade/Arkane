#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Core/Defines.h"
#include "../Core/SmartPointers.h"

#include "Shader.h"

#include <string>
#include <map>


#pragma warning(disable : 4251)

AK_NAMESPACE_BEGIN

class Device;
class AK_DLL ShaderManager final
{
public:
	static ShaderManager& GetInstance();

	SharedPtr<Shader> Load(SharedPtr<Device> _device, const std::string& _path);

	void Destroy(const std::string& _path);
	void Destroy(uint32_t _id);
	void DestroyAll();

	SharedPtr<Shader> GetShader(const std::string& _path) const;
	SharedPtr<Shader> GetShader(uint32_t _id) const;

private:
	ShaderManager();
	~ShaderManager();

private:
	std::map<uint32_t, SharedPtr<Shader>> m_shaders;
};

AK_NAMESPACE_END