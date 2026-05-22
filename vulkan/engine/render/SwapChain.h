#pragma once
#include "vulkan/vulkan.h"
#include <vector>
#include <optional>
#include "GLFW/glfw3.h"
#include <algorithm>
struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

class SwapChain
{
	VkDevice device = 0;
	VkSwapchainKHR swapChain =0;
	//多块画布
	std::vector<VkImage> swapChainImages;
	std::vector<VkImageView> swapChainImageViews;
	//同步对象
	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;
	std::vector<VkFence> imagesInFlight;
	
	VkFormat swapChainImageFormat ;
	VkFormat swapChainDepthFormat;
	VkExtent2D swapChainExtent = {0};

	std::vector<VkFramebuffer> swapChainFramebuffers;
public:
	VkRenderPass renderPass =0;
	VkSwapchainKHR getSwapchain() 
	{
		return swapChain;
	}
	void init();
	void createSwapChain();
	void createImageViews();
	void createDepthResources();
	void createRenderPass();
	void createFramebuffers();
	void createSyncObjects();
	void recreateSwapchain();
	void clean();
	void cleanUpSwapchain();
	uint32_t getImageCount() {
		return swapChainImages.size();
	}
	VkFramebuffer getFrameBuffer(int index) { return swapChainFramebuffers[index]; }
	VkImageView getImageView(int index) { return swapChainImageViews[index]; }
	VkExtent2D getExtent() {
		return swapChainExtent;
	}
	static SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);
	static VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	static VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window);
	static VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkResult acquireNextImage(uint32_t* imgIndex);
	VkResult submitCmd(VkCommandBuffer* buffers,uint32_t cmdCount, uint32_t* imageIndex);
	int currentFrame = 0;
};

