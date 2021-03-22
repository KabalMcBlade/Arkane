#include "EngineApp.h"

#include "../Core/Assertions.h"

#include "../Renderer/QueueFamily.h"
#include "../Renderer/Device.h"
#include "../Renderer/Instance.h"
#include "../Renderer/SwapChain.h"

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
	m_instance = new Instance(m_name, m_version);

	InitWindow();

	m_device = new Device(m_instance->GetInstance(), GetSurafe(), m_enabledFeatures);
	m_swapchain = new SwapChain(*m_device, GetSurafe(), GetWidth(), GetHeight());
}

void EngineApp::InternalInitEngine()
{
	InitEngine();
}

void EngineApp::InternalMainLoop()
{
	MainLoop();		// this has the loop
}

void EngineApp::InternalCleanup()
{
	delete m_swapchain;	// need to be cleared before surface
	delete m_device;

	Cleanup();

	delete m_instance;
}

AK_NAMESPACE_END