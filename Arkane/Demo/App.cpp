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


struct UniformBufferObjectData
{
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
};


// CREATE VERTEX AND INDEX
// TRIANGLE
/*
const std::vector<Vertex_C> _vertices =
{
	{{0.0f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},
	{{0.5f, 0.5f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}},
	{{-0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}}
};

const std::vector<uint16_t> _indices = {
	0, 1, 2
};
*/

// RECTANGLE
const std::vector<Vertex_C> _vertices =
{
	{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},
	{{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}},
	{{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}},
	{{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}}
};

const std::vector<uint16_t> _indices = {
	0, 1, 2, 2, 3, 0
};

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

	VkResult result = glfwCreateWindowSurface(RenderManager::GetInstance().GetRenderInstance()->GetInstance(), m_window, VulkanAllocator::GetInstance().GetCallbacks(), &m_surface);
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

bool App::CreateGraphicPipeline()
{
	bool result = false;

	//////////////////////////////////////////////////////////////////////////
	// CREATE SHADERS
	std::string vertpath = m_fileSystem->GetShadersPath() + "Simple.vert.spv";
	std::string fragpath = m_fileSystem->GetShadersPath() + "Simple.frag.spv";

	SharedPtr<Shader> testVert = ShaderManager::GetInstance().Load(RenderManager::GetInstance().GetDevice(), vertpath);
	SharedPtr<Shader> testFrag = ShaderManager::GetInstance().Load(RenderManager::GetInstance().GetDevice(), fragpath);
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	// CREATE VERTEXDESCRIPTOR
	SharedPtr<VertexDescriptor> vertex = MakeSharedPtr<VertexDescriptor>(EVertexLayout::EVertexLayout_C);
	//////////////////////////////////////////////////////////////////////////



	const size_t align = 16;
	const size_t mask = align - 1;
	//////////////////////////////////////////////////////////////////////////
	// CREATE VERTEX BUFFER OBJECT
	const size_t sizeVertex = ((_vertices.size() * sizeof(Vertex_C)) + mask) & ~mask;
	result = RenderManager::GetInstance().GetVBO()->AllocBufferObject(_vertices.data(), (uint32_t)sizeVertex, EBufferUsage::EBufferUsage_Dynamic);
	akAssertReturnValue(result == true, false, "Cannot allocated Vertex Buffer.");
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	// CREATE INDEX BUFFER OBJECT
	const size_t sizeIndex = ((_indices.size() * sizeof(uint16_t)) + mask) & ~mask;
	result = RenderManager::GetInstance().GetIBO()->AllocBufferObject(_indices.data(), (uint32_t)sizeIndex, EBufferUsage::EBufferUsage_Dynamic);
	akAssertReturnValue(result == true, false, "Cannot allocated Index Buffer.");
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	// CREATE DESCRIPTORSETLAYOUT
	RenderManager::GetInstance().GetDescriptorSetLayout()->Push(EDescriptorStage::EDescriptorStage_Vertex, EBindingType::EBindingType_Uniform, 0);
	result = RenderManager::GetInstance().GetDescriptorSetLayout()->Create();
	akAssertReturnValue(result == true, false, "Cannot create DescriptorSetLayout.");
	//////////////////////////////////////////////////////////////////////////


	const uint32_t swapChainCount = (uint32_t)RenderManager::GetInstance().GetSwapChainImageCount();


	//////////////////////////////////////////////////////////////////////////
	// CREATE DESCRIPTORPOOL
	RenderManager::GetInstance().GetDescriptorPool()->Push(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, swapChainCount);
	result = RenderManager::GetInstance().GetDescriptorPool()->Create(0, swapChainCount);
	akAssertReturnValue(result == true, false, "Cannot create DescriptorPool.");
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	// CREATE UNIFORM BUFFER OBJECT AND DESCRIPTORSETS
	const size_t sizeUbo = (sizeof(UniformBufferObjectData) + mask) & ~mask;

	UniformBufferObjectData ubo{};
	ubo.model = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.proj = glm::perspective(glm::radians(45.0f), (float)RenderManager::GetInstance().GetSwapChain()->GetExtent().width / (float)RenderManager::GetInstance().GetSwapChain()->GetExtent().height, 0.1f, 10.0f);
	ubo.proj[1][1] *= -1;

	for (uint32_t i = 0; i < swapChainCount; ++i)
	{
		if (RenderManager::GetInstance().GetUBO(i)->AllocBufferObject(&ubo, (uint32_t)sizeUbo, EBufferUsage::EBufferUsage_Dynamic))
		{
			RenderManager::GetInstance().GetDescriptorSet(i)->PushLayout(RenderManager::GetInstance().GetDescriptorSetLayout());

			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = RenderManager::GetInstance().GetUBO(i)->GetBufferObject();
			bufferInfo.offset = RenderManager::GetInstance().GetUBO(i)->GetOffset();
			bufferInfo.range = RenderManager::GetInstance().GetUBO(i)->GetAllocedSize();
			RenderManager::GetInstance().GetDescriptorSet(i)->PushDescriptor(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, &bufferInfo);

			result = RenderManager::GetInstance().GetDescriptorSet(i)->Create(RenderManager::GetInstance().GetDescriptorPool());
			akAssertReturnValue(result == true, false, "Cannot create DescriptorSet.");
		}
	}
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	// CREATE PIPELINE LAYOUT
	//RenderManager::GetInstance().GetPipelineLayout()->PushConstant();	// Optional, only if we have!
	result = RenderManager::GetInstance().GetPipelineLayout()->Create();
	akAssertReturnValue(result == true, false, "Cannot create PipelineLayout.");
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	// CREATE RENDERPASS
	result = RenderManager::GetInstance().CreateRenderPass(m_frameWidth, m_frameHeight);
	akAssertReturnValue(result == true, false, "Cannot create RenderPass.");
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	// CREATE GRAPHIC PIPELINE
	result = RenderManager::GetInstance().CreateGraphicsPipeline(vertex, testVert, testFrag);
	akAssertReturnValue(result == true, false, "Cannot create Pipeline.");
	//////////////////////////////////////////////////////////////////////////

	return result;
}

void App::RecordCommandBuffers()
{
	bool result = RenderManager::GetInstance().RecordCommandBuffers((uint32_t)_indices.size(), 1, 0, 0, 0);
	akAssertReturnVoid(result == true, "Impossible recording command buffer!");
}

void App::MainLoop()
{
	while (!glfwWindowShouldClose(m_window))
	{
		glfwPollEvents();

		BeginFrame();
		UpdateFrame();
		EndFrame();
	}
}

void App::UpdateFrame()
{
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	const uint32_t imageIndex = RenderManager::GetInstance().GetCurrentFrameImageIndex();

	UniformBufferObjectData ubo{};
	ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.proj = glm::perspective(glm::radians(45.0f), (float)RenderManager::GetInstance().GetSwapChain()->GetExtent().width / (float)RenderManager::GetInstance().GetSwapChain()->GetExtent().height, 0.1f, 10.0f);
	ubo.proj[1][1] *= -1;

	const size_t align = 16;
	const size_t mask = align - 1;
	const size_t sizeUbo = (sizeof(UniformBufferObjectData) + mask) & ~mask;
	RenderManager::GetInstance().GetUBO(imageIndex)->MapBuffer(EBufferMappingType::EBufferMappingType_Write);
	RenderManager::GetInstance().GetUBO(imageIndex)->Update(&ubo, (uint32_t)sizeUbo);
	RenderManager::GetInstance().GetUBO(imageIndex)->UnmapBuffer();
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
	ShaderManager::GetInstance().DestroyAll();

	vkDestroySurfaceKHR(RenderManager::GetInstance().GetRenderInstance()->GetInstance(), m_surface, VulkanAllocator::GetInstance().GetCallbacks());

	glfwDestroyWindow(m_window);

	glfwTerminate();
}