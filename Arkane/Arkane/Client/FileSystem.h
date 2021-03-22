#pragma once

#include "../Core/Defines.h"

#include <string>

#pragma warning( disable : 4251 )	//export std

AK_NAMESPACE_BEGIN

class AK_DLL FileSystem final
{
public:
	static void ConvertSlash(std::string& _path);
	static std::string ConvertSlash(const std::string& _path);
	static std::string GetFileName(const std::string& _path);
	static std::string GetFileExt(const std::string& _file);
	static std::string GetFileLeftMostExt(const std::string& _file);
	static std::string RemoveExt(const std::string& _file);

	// specific calls
public:
	static std::string GetShaderTypeExt(const std::string& _path);

public:
	FileSystem(const std::string& _mainPath,
		const std::string& _shadersPath,
		const std::string& _texturesPath,
		const std::string& _modelsPath);
	~FileSystem();

	const std::string& GetMainPath() const { return m_mainPath; }
	const std::string& GetShadersPath() const { return m_shadersPath; }
	const std::string& GetTexturesPath() const { return m_texturesPath; }
	const std::string& GetModelsPath() const { return m_modelsPath; }

private:
	bool GetFullPath(const std::string& partialPath, std::string& _fullPath);

private:
	std::string m_mainPath;
	std::string m_shadersPath;
	std::string m_texturesPath;
	std::string m_modelsPath;
};

AK_NAMESPACE_END