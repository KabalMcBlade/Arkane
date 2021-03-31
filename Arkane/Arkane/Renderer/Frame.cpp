#include "Frame.h"

#include "../Core/Assertions.h"
#include "../Core/VulkanAllocator.h"

#include "Device.h"
#include "SwapChain.h"
#include "CommandBuffer.h"


AK_NAMESPACE_BEGIN

size_t Frame::kNumOfFramesInFlight = 0;

Frame::Frame(SharedPtr<Device> _device, SharedPtr<SwapChain> _swapChain) : m_device(_device), m_currentFrame(0), m_imageIndex(0)
{
	m_imageAvailableSemaphores.resize(kNumOfFramesInFlight);
	m_renderFinishedSemaphores.resize(kNumOfFramesInFlight);
	m_inFlightFences.resize(kNumOfFramesInFlight);
	m_imagesInFlight.resize(_swapChain->GetImageViewsCount(), VK_NULL_HANDLE);

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < kNumOfFramesInFlight; ++i)
	{
		VkResult result = vkCreateSemaphore(m_device->GetDevice(), &semaphoreInfo, VulkanAllocator::GetInstance().GetCallbacks(), &m_imageAvailableSemaphores[i]);
		akAssertReturnVoid(result == VK_SUCCESS, "Failed to create semaphore for availability");

		result = vkCreateSemaphore(m_device->GetDevice(), &semaphoreInfo, VulkanAllocator::GetInstance().GetCallbacks(), &m_renderFinishedSemaphores[i]);
		akAssertReturnVoid(result == VK_SUCCESS, "Failed to create semaphore for finished render");

		result = vkCreateFence(m_device->GetDevice(), &fenceInfo, VulkanAllocator::GetInstance().GetCallbacks(), &m_inFlightFences[i]);
		akAssertReturnVoid(result == VK_SUCCESS, "Failed to create fence");
	}
}

Frame::~Frame()
{
	for (size_t i = 0; i < kNumOfFramesInFlight; ++i)
	{
		vkDestroySemaphore(m_device->GetDevice(), m_renderFinishedSemaphores[i], VulkanAllocator::GetInstance().GetCallbacks());
		vkDestroySemaphore(m_device->GetDevice(), m_imageAvailableSemaphores[i], VulkanAllocator::GetInstance().GetCallbacks());
		vkDestroyFence(m_device->GetDevice(), m_inFlightFences[i], VulkanAllocator::GetInstance().GetCallbacks());
	}
}

EFrameStatus Frame::BeginDraw(SharedPtr<SwapChain> _swapChain)
{
	VkResult result = vkWaitForFences(m_device->GetDevice(), 1, &m_inFlightFences[m_currentFrame], VK_TRUE, UINT64_MAX);
	akAssertReturnValue(result == VK_SUCCESS, EFrameStatus::EFrameStatus_Error, "Cannot wait for fences");

	result = vkAcquireNextImageKHR(m_device->GetDevice(), _swapChain->GetSwapChain(), UINT64_MAX, m_imageAvailableSemaphores[m_currentFrame], VK_NULL_HANDLE, &m_imageIndex);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_NOT_READY)
	{
		return EFrameStatus::EFrameStatus_NeedUpdate;
	}

	return EFrameStatus_Success;
}

EFrameStatus Frame::EndDraw(SharedPtr<SwapChain> _swapChain, std::vector<SharedPtr<CommandBuffer>>& _commandBuffers)
{
	VkResult result = VK_SUCCESS;

	if (m_imagesInFlight[m_imageIndex] != VK_NULL_HANDLE)
	{
		result = vkWaitForFences(m_device->GetDevice(), 1, &m_imagesInFlight[m_imageIndex], VK_TRUE, UINT64_MAX);
		akAssertReturnValue(result == VK_SUCCESS, EFrameStatus::EFrameStatus_Error, "Cannot wait for fences");
	}

	m_imagesInFlight[m_imageIndex] = m_inFlightFences[m_currentFrame];

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { m_imageAvailableSemaphores[m_currentFrame] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &_commandBuffers[m_imageIndex]->GetCommandBuffer();

	VkSemaphore signalSemaphores[] = { m_renderFinishedSemaphores[m_currentFrame] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	result = vkResetFences(m_device->GetDevice(), 1, &m_inFlightFences[m_currentFrame]);
	akAssertReturnValue(result == VK_SUCCESS, EFrameStatus::EFrameStatus_Error, "Cannot reset fences");

	//////////////////////////////////////////////////////////////////////////

	result = vkQueueSubmit(m_device->GetGraphicsQueue(), 1, &submitInfo, m_inFlightFences[m_currentFrame]);
	akAssertReturnValue(result == VK_SUCCESS, EFrameStatus::EFrameStatus_Error, "Failed to submit draw command buffer");

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { _swapChain->GetSwapChain() };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;

	presentInfo.pImageIndices = &m_imageIndex;

	result = vkQueuePresentKHR(m_device->GetPresentQueue(), &presentInfo);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
	{
		return EFrameStatus_NeedUpdate;
	}

	m_currentFrame = (m_currentFrame + 1) % kNumOfFramesInFlight;

	return EFrameStatus_Success;
}


AK_NAMESPACE_END