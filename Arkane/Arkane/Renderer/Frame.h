#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Core/Defines.h"
#include "../Core/SmartPointers.h"


#include <memory>
#include <vector>


#pragma warning(disable:4251)

AK_NAMESPACE_BEGIN

enum class EFrameStatus
{
	EFrameStatus_Success = 0,
	EFrameStatus_NeedUpdate,
	EFrameStatus_Error
};

class Device;
class CommandBuffer;
class SwapChain;
class AK_DLL Frame final
{
public:
	static size_t kNumOfFramesInFlight;

public:
	Frame(SharedPtr<Device> _device, SharedPtr<SwapChain> _swapChain);
	~Frame();

	AK_INLINE uint32_t GetCurrentImageIndex() const { return m_imageIndex; }

	EFrameStatus BeginDraw(SharedPtr<SwapChain> _swapChain);
	EFrameStatus EndDraw(SharedPtr<SwapChain> _swapChain, std::vector<SharedPtr<CommandBuffer>>& _commandBuffers);

private:
	SharedPtr<Device> m_device;

	std::vector<VkSemaphore> m_imageAvailableSemaphores;
	std::vector<VkSemaphore> m_renderFinishedSemaphores;
	std::vector<VkFence> m_inFlightFences;
	std::vector<VkFence> m_imagesInFlight;
	uint32_t m_imageIndex;
	size_t m_currentFrame;
};

AK_NAMESPACE_END