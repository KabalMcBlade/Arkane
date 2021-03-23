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
	m_commandLine->GetValue("-width", m_width);
	m_commandLine->GetValue("-height", m_height);

	glfwInit();

	int isVulkanSupported = glfwVulkanSupported();
	akAssertReturnVoid(isVulkanSupported == GLFW_TRUE, "No Vulkan Support.");

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	bool fullScreen;
	m_commandLine->GetValue("-fullscreen", fullScreen);

	bool showcursor;
	m_commandLine->GetValue("-showcursor", showcursor);

	if (fullScreen)
	{
		m_window = glfwCreateWindow(m_width, m_height, m_name, glfwGetPrimaryMonitor(), nullptr);
	}
	else
	{
		m_window = glfwCreateWindow(m_width, m_height, m_name, nullptr, nullptr);
	}

	if (!showcursor)
	{
		glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

	glfwGetFramebufferSize(m_window, &m_frameWidth, &m_frameheight);

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
	bool result = false;

	Device& currentDevice = *m_device;

	/*
	//////////////////////////////////////////////////////////////////////////
	// Sample load Shader (just to remind me relative path management)
	{
		std::string vertpath = m_fileSystem->GetShadersPath() + "ShaderTest.vert.spv";
		std::string fragpath = m_fileSystem->GetShadersPath() + "ShaderTest.frag.spv";

		SharedPtr<Shader> testVert = ShaderManager::Instance().Load(currentDevice, vertpath);
		SharedPtr<Shader> testFrag = ShaderManager::Instance().Load(currentDevice, fragpath);

		ShaderManager::Instance().Destroy(vertpath);
		ShaderManager::Instance().Destroy(fragpath);
	}
	*/

	VertexDescriptor vertex(EVertexLayout::EVertexLayout_C);



	/*
	
	JUST TO KEEP TRACK OF A SIMPLER FIRST SETTING! IS NOT GOING TO BE PLACED HERE!

	*/

	//////////////////////////////////////////////////////////////////////////
	// CREATE RENDERPASS
	RenderPass renderPass(currentDevice);

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

	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;


	renderPass.SetDimensions(m_width, m_height);
	renderPass.SetOffset(0, 0);
	renderPass.PushColorDepthStencilClearValue(0.0f, 0.0f, 0.0f, 1.0f);
	renderPass.PushAttachmentDescription(colorAttachment);
	renderPass.PushSubpassDependency(dependency);
	renderPass.PushSubpassDescription(subpass);

	// NOTE: the FrameBuffer is need it ONLY for the renderPass.GetBeginInfo()
	//renderPass.SetFrameBuffer(m_framebuffer);

	result = renderPass.Create();
	akAssertReturnVoid(result == true, "Cannot create RenderPass, Init Engine Failed.");
	//////////////////////////////////////////////////////////////////////////
}

void App::MainLoop()
{
	while (!glfwWindowShouldClose(m_window))
	{
		glfwPollEvents();
	}
}

void App::Cleanup()
{
	vkDestroySurfaceKHR(m_instance->GetInstance(), m_surface, VulkanAllocator::Instance().GetCallbacks());

	glfwDestroyWindow(m_window);

	glfwTerminate();
}