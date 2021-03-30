#include "App.h"


#pragma warning(disable : 26812)

AK_USING_NAMESPACE


static void KeyCallback(GLFWwindow* _window, int _key, int _scancode, int _action, int _mods)
{
	if (_key == GLFW_KEY_ESCAPE && _action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(_window, GLFW_TRUE);
	}
}


//////////////////////////////////////////////////////////////////////////

void App::InitWindow()
{
	uint32_t width;
	uint32_t height;

	m_commandLine->GetValue("-width", width);
	m_commandLine->GetValue("-height", height);

	glfwInit();

	int isVulkanSupported = glfwVulkanSupported();
	akAssertReturnVoid(isVulkanSupported == GLFW_TRUE, "No Vulkan Support.");

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	bool fullScreen;
	m_commandLine->GetValue("-fullscreen", fullScreen);

	bool showcursor;
	m_commandLine->GetValue("-showcursor", showcursor);

	if (fullScreen)
	{
		m_window = glfwCreateWindow(width, height, m_name, glfwGetPrimaryMonitor(), nullptr);
	}
	else
	{
		m_window = glfwCreateWindow(width, height, m_name, nullptr, nullptr);
	}

	if (!showcursor)
	{
		glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

	glfwGetFramebufferSize(m_window, &m_frameWidth, &m_frameHeight);

	glfwSetKeyCallback(m_window, KeyCallback);

	glfwMakeContextCurrent(m_window);

	VkResult result = glfwCreateWindowSurface(m_instance->GetInstance(), m_window, VulkanAllocator::Instance().GetCallbacks(), &m_surface);
	akAssertReturnVoid(result == VK_SUCCESS, "Failed to create window surface.");

	m_enabledFeatures = {};
	m_enabledFeatures.shaderStorageImageExtendedFormats = VK_TRUE;
	m_enabledFeatures.geometryShader = VK_TRUE;
}

void App::InitEngine()
{
	bool result = CreateGraphicPipeline();
	akAssertReturnVoid(result == true, "Impossible Initialize the engine!");
}


//////////////////////////////////////////////////////////////////////////
// CREATE VERTEX
const std::vector<Vertex_C> _vertices =
{
	{{0.0f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},
	{{0.5f, 0.5f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}},
	{{-0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}}
};
//////////////////////////////////////////////////////////////////////////

bool App::CreateGraphicPipeline()
{
	bool result = false;

	//////////////////////////////////////////////////////////////////////////
	// CREATE SHADERS
	std::string vertpath = m_fileSystem->GetShadersPath() + "Simple.vert.spv";
	std::string fragpath = m_fileSystem->GetShadersPath() + "Simple.frag.spv";

	SharedPtr<Shader> testVert = ShaderManager::Instance().Load(m_device, vertpath);
	SharedPtr<Shader> testFrag = ShaderManager::Instance().Load(m_device, fragpath);
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	// CREATE VERTEXDESCRIPTOR
	SharedPtr<VertexDescriptor> vertex = MakeSharedPtr<VertexDescriptor>(EVertexLayout::EVertexLayout_C);
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	// CREATE VERTEX BUFFER OBJECT
	const size_t align = 16;
	const size_t mask = align - 1;
	const size_t size = ((_vertices.size() * sizeof(Vertex_C)) + mask) & ~mask;

	void* vboMemory = nullptr;
	m_vbo = MakeSharedPtr<VertexBufferObject>();
	if (m_vbo->AllocBufferObject(_vertices.data(), (uint32_t)size, Arkane::EBufferUsage::EBufferUsage_Dynamic))
	{
		vboMemory = m_vbo->MapBuffer(Arkane::EBufferMappingType::EBufferMappingType_Write);

		m_vbo->Update(_vertices.data(), (uint32_t)size);
		m_vbo->UnmapBuffer();
	}
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	// CREATE DESCRIPTORSETLAYOUT
	SharedPtr<DescriptorSetLayout> layout = MakeSharedPtr<DescriptorSetLayout>(m_device);
	layout->Push(Arkane::EDescriptorStage_Vertex, Arkane::EBindingType_Uniform, 0);

	result = layout->Create();
	akAssertReturnValue(result == true, false, "Cannot create DescriptorSetLayout.");
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	// CREATE PIPELINE LAYOUT
	SharedPtr<PipelineLayout> pipelineLayout = MakeSharedPtr<PipelineLayout>(m_device, layout);
	//pipelineLayout->PushConstant();	// Optional, only if we have!
	result = pipelineLayout->Create();
	akAssertReturnValue(result == true, false, "Cannot create PipelineLayout.");
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	// CREATE RENDERPASS
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = m_swapchain->GetFormat();
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;

	VkSubpassDependency dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	m_renderPass->SetDimensions(m_frameWidth, m_frameHeight);
	m_renderPass->SetOffset(0, 0);
	m_renderPass->PushColorDepthStencilClearValue(0.0f, 0.0f, 0.0f, 1.0f);
	m_renderPass->PushAttachmentDescription(colorAttachment);
	m_renderPass->PushSubpassDependency(dependency);
	m_renderPass->PushSubpassDescription(subpass);
	//m_renderPass->SetFrameBuffer(m_framebuffer);		// NEVER CALL THIS FUNCTION HERE! CALL FROM CommandBuffer!

	result = m_renderPass->Create();
	akAssertReturnValue(result == true, false, "Cannot create RenderPass.");
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	// CREATE PIEPLINE
	m_pipeline->SetVertexInput(vertex);
	m_pipeline->SetTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
	m_pipeline->SetPolygonMode(VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_CLOCKWISE);
	m_pipeline->SetDepthStencil(VK_FALSE, VK_FALSE);
	m_pipeline->SetViewport(m_swapchain);
	m_pipeline->SetMultisample(VK_FALSE, VK_SAMPLE_COUNT_1_BIT);

	// Call Set without push will not add anything but will create the structure, which is need it
	//m_pipeline->PushDynamic();	
	m_pipeline->SetDynamics();

	m_pipeline->PushColorBlendAttachment(VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT, VK_FALSE);
	m_pipeline->SetColorBlendAttachments();

	m_pipeline->SetLayout(pipelineLayout);
	m_pipeline->SetRenderPass(m_renderPass);
	m_pipeline->SetCache(m_pipelineCache);

	m_pipeline->PushShader(testVert);
	m_pipeline->PushShader(testFrag);

	result = m_pipeline->Create();
	akAssertReturnValue(result == true, false, "Cannot create Pipeline.");
	//////////////////////////////////////////////////////////////////////////

	return result;
}

bool App::RecordCommandBuffers()
{
	bool result = true;
	for (size_t i = 0; i < m_commandBuffers.size(); i++)
	{
		result = m_commandBuffers[i]->Begin();
		akAssertReturnValue(result == true, false, "Cannot begin command buffer.");

		m_commandBuffers[i]->BeginRenderPass(m_renderPass, m_frameBuffers[i]->GetFrameBuffer());

		m_commandBuffers[i]->BindPipeline(m_pipeline);

		m_commandBuffers[i]->BindVertexBuffer(m_vbo, 0, 0);
		m_commandBuffers[i]->Draw((uint32_t)_vertices.size(), 1, 0, 0);

		m_commandBuffers[i]->EndRenderPass();

		result = m_commandBuffers[i]->End();
		akAssertReturnValue(result == true, false, "Failed to record command buffer.");
	}

	return result;
}

void App::MainLoop()
{
	bool result = RecordCommandBuffers();
	akAssertReturnVoid(result == true, "Impossible Initialize the engine!");

	while (!glfwWindowShouldClose(m_window))
	{
		glfwPollEvents();
		DrawFrame();
	}

	// probably is better in the Cleanup(), but Cleanup)( is called too late, check a new order
	// This return a validation error, because the Buffer need to finish the full execution before freed! 
	// Maybe need a pre/post clean up and pore/post of all the functions App has!
	m_vbo->FreeBufferObject();	
	//m_vbo.reset();
}

void App::Recreate()
{
	glfwGetFramebufferSize(m_window, &m_frameWidth, &m_frameHeight);
	while (m_frameWidth == 0 || m_frameHeight == 0)
	{
		glfwGetFramebufferSize(m_window, &m_frameWidth, &m_frameHeight);
		glfwWaitEvents();
	}
}

void App::Cleanup()
{
	ShaderManager::Instance().DestroyAll();

	vkDestroySurfaceKHR(m_instance->GetInstance(), m_surface, VulkanAllocator::Instance().GetCallbacks());

	glfwDestroyWindow(m_window);

	glfwTerminate();
}