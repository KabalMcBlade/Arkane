#include "FileSystem.h"


AK_NAMESPACE_BEGIN

void FileSystem::ConvertSlash(std::string& _path)
{
	static const std::string from = "\\";
	static const std::string to = "/";

	size_t start_pos = 0;
	while ((start_pos = _path.find(from, start_pos)) != std::string::npos)
	{
		_path.replace(start_pos, from.length(), to);
		start_pos += to.length();
	}
}

std::string FileSystem::ConvertSlash(const std::string& _path)
{
	static const std::string from = "\\";
	static const std::string to = "/";

	std::string result = _path;

	size_t start_pos = 0;
	while ((start_pos = result.find(from, start_pos)) != std::string::npos)
	{
		result.replace(start_pos, from.length(), to);
		start_pos += to.length();
	}

	return result;
}

std::string FileSystem::GetFileName(const std::string& _path)
{
	std::string path = ConvertSlash(_path);

	size_t i = path.find_last_of("/");
	if (i != std::string::npos)
	{
		return path.substr(i + 1);
	}

	return("");
}

std::string FileSystem::GetFileExt(const std::string& _file)
{
	size_t i = _file.rfind('.', _file.length());
	if (i != std::string::npos)
	{
		return(_file.substr(i + 1, _file.length() - i));
	}

	return("");
}

std::string FileSystem::GetFileLeftMostExt(const std::string& _file)
{
	std::string file = ConvertSlash(_file);

	size_t index = file.find_last_of("/");
	file = file.substr(index + 1);

	size_t i = file.find('.', 0);
	if (i != std::string::npos)
	{
		return(file.substr(i + 1, file.length() - i));
	}

	return("");
}

std::string FileSystem::RemoveExt(const std::string& _file)
{
	size_t i = _file.find_last_of(".");
	if (i != std::string::npos)
	{
		return(_file.substr(0, i));
	}

	return("");
}

std::string FileSystem::GetShaderTypeExt(const std::string& _path)
{
	std::string leftmostExt = Arkane::FileSystem::GetFileLeftMostExt(_path);
	return Arkane::FileSystem::RemoveExt(leftmostExt);
}


//////////////////////////////////////////////////////////////////////////

FileSystem::FileSystem(const std::string& _mainPath,
	const std::string& _shadersPath,
	const std::string& _texturesPath,
	const std::string& _modelsPath)
{
	GetFullPath("./", m_mainPath);

	ConvertSlash(m_mainPath);

	m_mainPath.append(_mainPath);
	m_mainPath.append("/");

	m_shadersPath = m_mainPath;
	m_shadersPath.append(_shadersPath);
	m_shadersPath.append("/");

	m_texturesPath = m_mainPath;
	m_texturesPath.append(_texturesPath);
	m_texturesPath.append("/");

	m_modelsPath = m_mainPath;
	m_modelsPath.append(_modelsPath);
	m_modelsPath.append("/");
}

FileSystem::~FileSystem()
{
}

bool FileSystem::GetFullPath(const std::string& partialPath, std::string& _fullPath)
{
	char tmp[_MAX_PATH];
	if (_fullpath(tmp, partialPath.c_str(), _MAX_PATH) != NULL)
	{
		_fullPath = tmp;
		return true;
	}
	return false;
}

AK_NAMESPACE_END