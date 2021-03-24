#include "EngineApp.h"

#include "../Core/Assertions.h"

#include "../Renderer/QueueFamily.h"
#include "../Renderer/Device.h"
#include "../Renderer/Instance.h"
#include "../Renderer/SwapChain.h"
#include "../Renderer/RenderPass.h"
#include "../Renderer/PipelineCache.h"
#include "../Renderer/Pipeline.h"
#include "../Renderer/FrameBuffer.h"
#include "../Renderer/CommandPool.h"

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

	m_commandPool = MakeSharedPtr<CommandPool>(m_device, (uint32_t)m_device->GetQueueFamily()->GetGraphicsFamily());

	m_renderPass = MakeSharedPtr<RenderPass>(m_device);
	m_pipelineCache = MakeSharedPtr<PipelineCache>(m_device);
	m_pipeline = MakeSharedPtr<Pipeline>(m_device);

	// here should be custom initialization and also set the render pass
	InitEngine();
}

void EngineApp::InternalMainLoop()
{
	std::vector<SharedPtr<FrameBuffer>>::size_type count = m_swapchain->GetImageViewsCount();
	m_frameBuffers.resize(count);

	for (std::vector<SharedPtr<FrameBuffer>>::size_type i = 0; i < count; ++i)
	{
		m_frameBuffers[i] = MakeSharedPtr<FrameBuffer>(m_device, m_renderPass, GetWidth(), GetHeight(), 1);
		m_frameBuffers[i]->PushAttachment(m_swapchain->GetImageView(i));
		m_frameBuffers[i]->Create();
	}

	MainLoop();		// this has the loop
}

void EngineApp::InternalCleanup()
{
	// I need to manually force the delete because I need this order!
	for (std::vector<SharedPtr<FrameBuffer>>::size_type i = 0; i < m_swapchain->GetImageViewsCount(); ++i)
	{
		m_frameBuffers[i].reset();
	}
	m_frameBuffers.clear();

	m_pipeline.reset();
	m_pipelineCache.reset();
	m_renderPass.reset();

	m_commandPool.reset();

	m_swapchain.reset();
	m_device.reset();

	Cleanup();

	m_instance.reset();
}

AK_NAMESPACE_END