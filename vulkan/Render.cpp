#include "Render.h"

void RenderManager::beginFrame()
{
    auto result = swapchain->acquireNextImage(&currentImageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        swapchain->recreateSwapchain();
        return nullptr;
    }

    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    isFrameStarted = true;

    auto commandBuffer = getCurrentCommandBuffer();
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }
    return commandBuffer;
}

void RenderManager::endFrame()
{

}

void RenderManager::beginRenderPass(VkCommandBuffer commandBuffer)
{
}

void RenderManager::endRenderPass(VkCommandBuffer commandBuffer)
{
}
