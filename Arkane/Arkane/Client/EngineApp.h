#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>


#include "../Core/Defines.h"
#include "../Core/SmartPointers.h"


#include <memory>
#include <string>
#include <vector>


#pragma warning(disable:4251)

AK_NAMESPACE_BEGIN

class CommandLineParser;
class FileSystem;
class Instance;
class Device;
class SwapChain;
class RenderPass;
class Pipeline;
class PipelineCache;
class FrameBuffer;
class CommandPool;
class CommandBuffer;
class Frame;
class AK_DLL EngineApp
{
public:
	EngineApp(const char* _name, uint32_t _version, const CommandLineParser* _commandLine, const FileSystem* _fileSystem);
	~EngineApp();

	void Execute();

protected:
	virtual const VkSurfaceKHR& GetSurafe() const = 0;
	virtual int32_t GetFrameWidth() const = 0;
	virtual int32_t GetFrameHeight() const = 0;

	// custom command buffers creation on the App side
	virtual bool RecordCommandBuffers() = 0;


protected:
	virtual void InitWindow() {}
	virtual void InitEngine() {}
	virtual void MainLoop() {}
	virtual void Cleanup() {}
	virtual void Recreate() {}

protected:
	void DrawFrame();

private:
	void InternalInitWindow();
	void InternalInitEngine();
	void InternalMainLoop();
	void InternalCleanup();
	void InternalRecreate();


protected:
	const CommandLineParser* m_commandLine;
	const FileSystem* m_fileSystem;

	SharedPtr<Instance> m_instance;
	SharedPtr<Device> m_device;
	SharedPtr<SwapChain> m_swapchain;
	SharedPtr<CommandPool> m_commandPool;
	SharedPtr<RenderPass> m_renderPass;
	SharedPtr<PipelineCache> m_pipelineCache;
	SharedPtr<Pipeline> m_pipeline;
	SharedPtr<Frame> m_frame;
	std::vector<SharedPtr<FrameBuffer>> m_frameBuffers;
	std::vector<SharedPtr<CommandBuffer>> m_commandBuffers;

	VkPhysicalDeviceFeatures m_enabledFeatures;
	const char* m_name;
	uint32_t m_version;
};

AK_NAMESPACE_END