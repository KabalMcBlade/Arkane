#pragma once

#include "../Arkane/Arkane.h"


#define GLFW_INCLUDE_VULKAN
#include "Dependencies/glfw/include/GLFW/glfw3.h"
#include "Dependencies/glfw/include/GLFW/glfw3native.h"


AK_USING_NAMESPACE

class App final : public EngineApp
{
public:
	App(const char* _name, uint32_t _version, const CommandLineParser* _commandLine, const FileSystem* _fileSystem) : EngineApp(_name, _version, _commandLine, _fileSystem) 
	, m_surface(0)
	, m_window(nullptr) 
	, m_frameWidth(0)
	, m_frameHeight(0) {}
	~App() {}

	AK_INLINE const GLFWwindow& GetWindow() const { return *m_window; }


	virtual int32_t GetFrameWidth() const override { return m_frameWidth; }
	virtual int32_t GetFrameHeight() const override { return m_frameHeight; }

	virtual const VkSurfaceKHR& GetSurafe() const override { return m_surface; }

	virtual bool RecordCommandBuffers() override;

	virtual void InitWindow() override;
	virtual void InitEngine() override;
	virtual void MainLoop() override;
	virtual void Cleanup() override;
	virtual void Recreate() override;

private:
	bool CreateGraphicPipeline();

private:
	VkSurfaceKHR m_surface;
	GLFWwindow* m_window;

	SharedPtr<VertexBufferObject> m_vbo;
	SharedPtr<IndexBufferObject> m_ibo;

	int32_t m_frameWidth;
	int32_t m_frameHeight;
};

