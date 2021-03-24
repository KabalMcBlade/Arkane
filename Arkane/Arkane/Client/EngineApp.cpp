#include "EngineApp.h"

#include "../Core/Assertions.h"

#include "../Renderer/QueueFamily.h"
#include "../Renderer/Device.h"
#include "../Renderer/Instance.h"
#include "../Renderer/SwapChain.h"
#include "../Renderer/RenderPass.h"

#include "CommandLineParser.h"
#include "FileSystem.h"


AK_NAMESPACE_BEGIN

EngineApp::EngineApp(const char* _name, uint32_t _version, const CommandLineParser* _commandLine, const FileSystem* _fileSystem)
	: m_name(_name)
	, m_version(_version)
	, m_commandLine(_commandLine)
	, m_fileSystem(_fileSystem)
	, m_enabledFeatures({})
{

}

EngineApp::~EngineApp()
{

}

void EngineApp::Execute()
{
	InternalInitWindow();
	InternalInitEngine();
	InternalMainLoop();		// this has the loop
	InternalCleanup();
}

void EngineApp::InternalInitWindow()
{
	m_instance = MakeSharedPtr<Instance>(m_name, m_version);

	InitWindow();

}

void EngineApp::InternalInitEngine()
{
	m_device = MakeSharedPtr<Device>(m_instance->GetInstance(), GetSurafe(), m_enabledFeatures);
	m_swapchain = MakeSharedPtr<SwapChain>(m_device, GetSurafe(), GetWidth(), GetHeight());

	m_renderPass = MakeSharedPtr<RenderPass>(m_device);

	InitEngine();
}

void EngineApp::InternalMainLoop()
{
	MainLoop();		// this has the loop
}

void EngineApp::InternalCleanup()
{
	// I need to manually force the delete because I need this order!
	m_renderPass.reset();

	m_swapchain.reset();
	m_device.reset();

	Cleanup();

	m_instance.reset();
}

AK_NAMESPACE_END