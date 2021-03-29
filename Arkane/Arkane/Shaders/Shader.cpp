#include "Shader.h"

#include "../Core/Assertions.h"
#include "../Core/VulkanAllocator.h"
#include "../Client/FileSystem.h"
#include "../Utilities/Hash.h"


#include <fstream>


AK_NAMESPACE_BEGIN

VkShaderStageFlagBits Shader::ConvertExtToShaderStage(const std::string& _ext)
{
	if (_ext == "vert")
	{
		return VK_SHADER_STAGE_VERTEX_BIT;
	}
	if (_ext == "ctrl")
	{
		return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
	}
	if (_ext == "eval")
	{
		return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
	}
	if (_ext == "geom")
	{
		return VK_SHADER_STAGE_GEOMETRY_BIT;
	}
	if (_ext == "frag")
	{
		return VK_SHADER_STAGE_FRAGMENT_BIT;
	}
	if (_ext == "comp")
	{
		return VK_SHADER_STAGE_COMPUTE_BIT;
	}

	akAssertReturnValue(false, VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM, "No Valid conversion found!");

	// this is needed in release
	return VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
}

Shader::Shader(SharedPtr<Device> _device, const std::string& _path) : m_device(_device), m_module(VK_NULL_HANDLE)
{
	std::string type = FileSystem::GetShaderTypeExt(_path);

	m_stage = ConvertExtToShaderStage(type);

	std::ifstream fileStream(_path.c_str(), std::ios::binary);

	fileStream.seekg(0, std::ios_base::end);
	const size_t fileSize = fileStream.tellg();

	akAssertReturnVoid(fileSize > 0, "Shader file size is 0!");
	akAssertReturnVoid(fileSize % sizeof(uint32_t) == 0, "Not valid file buffer");

	char* binary = new char[fileSize];

	fileStream.seekg(0, std::ios_base::beg);
	fileStream.read(binary, fileSize);

	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = fileSize;
	createInfo.pCode = reinterpret_cast<const uint32_t*>(binary);

	VkResult result = vkCreateShaderModule(m_device->GetDevice(), &createInfo, VulkanAllocator::Instance().GetCallbacks(), &m_module);

	delete [] binary;

	akAssertReturnVoid(result == VK_SUCCESS, "Cannot create shader!");

	m_hash = akHashEx(_path.c_str(), _path.length());
}

Shader::~Shader()
{
	if (m_device != VK_NULL_HANDLE && m_module != VK_NULL_HANDLE)
	{
		vkDestroyShaderModule(m_device->GetDevice(), m_module, VulkanAllocator::Instance().GetCallbacks());
		//m_module = VK_NULL_HANDLE;
	}
}

AK_NAMESPACE_END