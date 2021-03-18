#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>


#include "../Core/BasicDefines.h"

#include "../Renderer/QueueFamily.h"
#include "../Renderer/Device.h"
#include "../Renderer/Instance.h"

#include <memory>
#include <string>


#pragma warning(disable:4251)

AK_NAMESPACE_BEGIN

class CommandLineParser;
class FileSystem;
class AK_DLL EngineApp
{
public:
	EngineApp(const char* _name, uint32_t _version, const CommandLineParser* _commandLine, const FileSystem* _fileSystem);
	~EngineApp();

	void Execute();

protected:
	virtual const VkSurfaceKHR& GetSurafe() const = 0;

protected:
	virtual void InitWindow() {}
	virtual void InitEngine() {}
	virtual void MainLoop() {}
	virtual void Cleanup() {}

private:
	void InternalInitWindow();
	void InternalInitEngine();
	void InternalMainLoop();
	void InternalCleanup();


protected:
	const CommandLineParser* m_commandLine;	// keep unique
	const FileSystem* m_fileSystem;			// keep unique
	Instance* m_instance;					// need to be shared with other
	Device* m_device;						// need to be shared with other
	VkPhysicalDeviceFeatures m_enabledFeatures;
	const char* m_name;
	uint32_t m_version;
};

AK_NAMESPACE_END