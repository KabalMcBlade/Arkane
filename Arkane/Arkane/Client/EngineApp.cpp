#include "EngineApp.h"

#include "../Core/Assertions.h"
#include "../Core/VulkanAllocator.h"

#include "../Renderer/RenderManager.h"

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
	RenderManager::GetInstance().CreateInstance(m_name, m_version);

	InitWindow();
}

void EngineApp::InternalInitEngine()
{
	RenderManager::GetInstance().CreateCore(GetSurafe(), GetFrameWidth(), GetFrameHeight(), m_enabledFeatures);

	// here should be custom initialization and also set the render pass
	InitEngine();
}

void EngineApp::InternalMainLoop()
{
	RenderManager::GetInstance().CreateBuffers(GetFrameWidth(), GetFrameHeight(), 1);

	RenderManager::GetInstance().CreateFrame();

	RecordCommandBuffers();

	// This has the main core loop of the game
	MainLoop();

	RenderManager::GetInstance().WaitIdle();

	RenderManager::GetInstance().DestroyFrame();
}

void EngineApp::BeginFrame()
{
	EFrameStatus status = RenderManager::GetInstance().BeginFrame();
	switch (status)
	{
	case Arkane::EFrameStatus_Success:
		// OK, keep to show it.
		break;
	case Arkane::EFrameStatus_NeedUpdate:
		InternalRecreate();
		break;
	case Arkane::EFrameStatus_Error:
		akAssertReturnVoid(false, "Drawing Error!");
		break;
	default:
		break;
	}
}

void EngineApp::EndFrame()
{
	EFrameStatus status = RenderManager::GetInstance().EndFrame();
	switch (status)
	{
	case Arkane::EFrameStatus_Success:
		// OK, keep to show it.
		break;
	case Arkane::EFrameStatus_NeedUpdate:
		InternalRecreate();
		break;
	case Arkane::EFrameStatus_Error:
		akAssertReturnVoid(false, "Drawing Error!");
		break;
	default:
		break;
	}
}

void EngineApp::InternalRecreate()
{
	Recreate();

	RenderManager::GetInstance().WaitIdle();

	RenderManager::GetInstance().RecreateCore(GetSurafe(), GetFrameWidth(), GetFrameHeight());

	InitEngine();

	RenderManager::GetInstance().RecreateBuffers(GetFrameWidth(), GetFrameHeight(), 1);

	RecordCommandBuffers();
}

void EngineApp::InternalCleanup()
{
	RenderManager::GetInstance().DestroyFrame();
	RenderManager::GetInstance().DestroyBuffers();
	RenderManager::GetInstance().DestroyCore();

	Cleanup();

	RenderManager::GetInstance().DestroyInstance();
}

AK_NAMESPACE_END