#pragma once
#include "vulkan/vulkan.h"
#include "SwapChain.h"
class RenderManager
{

	void beginFrame();
	void endFrame();
	void beginRenderPass(VkCommandBuffer commandBuffer);
	void endRenderPass(VkCommandBuffer commandBuffer);
	SwapChain* swapchain;
	bool isFrameStarted = false;
	uint32_t currentImageIndex;
	std::vector<VkCommandBuffer> commandBuffers;

};


