#include "ShaderManager.h"


#include "../Core/Assertions.h"
#include "../Core/VulkanAllocator.h"
#include "../Utilities/Hash.h"
#include "../Renderer/Device.h"


AK_NAMESPACE_BEGIN

ShaderManager& ShaderManager::Instance()
{
	static ShaderManager instance;
	return instance;
}

SharedPtr<Shader> ShaderManager::Load(const Device& _device, const std::string& _path)
{
	if (_path.empty())
	{
		return nullptr;
	}

	SharedPtr<Shader> shader = GetShader(_path);
	if (shader == nullptr)
	{
		shader = MakeSharedPtr<Shader>(_device.GetDevice(), _path);
		m_shaders[shader->GetId()] = shader;
	}

	return shader;
}

void ShaderManager::Destroy(const std::string& _path)
{
	uint32_t hash = akHashEx(_path.c_str(), _path.length());
	Destroy(hash);
}

void ShaderManager::Destroy(uint32_t _id)
{
	auto search = m_shaders.find(_id);
	if (search != m_shaders.end())
	{
// 		if (search->second != nullptr)
// 		{
// 			delete search->second;
// 		}

		m_shaders.erase(_id);
	}
}

void ShaderManager::DestroyAll()
{
// 	auto begin = m_shaders.begin(), end = m_shaders.end();
// 	std::map<uint32_t, SharedPtr<Shader>>::iterator it = begin;
// 	for (; it != end; ++it)
// 	{
// 		if (it->second != nullptr)
// 		{
// 			delete it->second;
// 		}
// 	}

	m_shaders.clear();
}

SharedPtr<Shader> ShaderManager::GetShader(const std::string& _path) const
{
	if (_path.empty())
	{
		return nullptr;
	}

	uint32_t hash = akHashEx(_path.c_str(), _path.length());
	return GetShader(hash);
}

SharedPtr<Shader> ShaderManager::GetShader(uint32_t _id) const
{
	auto search = m_shaders.find(_id);
	if (search != m_shaders.end())
	{
		return search->second;
	}
	else
	{
		return nullptr;
	}
}

//////////////////////////////////////////////////////////////////////////

ShaderManager::ShaderManager()
{
}


ShaderManager::~ShaderManager()
{
}

AK_NAMESPACE_END