#include "SwapChain.h"
#include "../window/glWindow.h"
#include "Device.h"
#include "Texture.h"
void SwapChain::init()
{
    device = Device::getInstance().device;
    createSwapChain();
    createImageViews();
    createRenderPass();
    createDepthResources();
    createFramebuffers();
    createSyncObjects();
}

void SwapChain::createSwapChain()
{
    //这是一块画布，也是显示器和GPU之间的桥梁。
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(Device::getInstance().gpu, Device::getInstance().surface);

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR; //chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities, glWindow::getInstance().getGLFWwindow());

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }
    //做一些检查来确定支持情况

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = Device::getInstance().surface;

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = Utils::findQueueFamilies(Device::getInstance().gpu, Device::getInstance().surface);
    uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

    if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    createInfo.oldSwapchain = VK_NULL_HANDLE;
    //创建一个swapchain
    Utils::vk_check(vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain));

    //获取swapchain中的image  这些image就是我们要渲染的画布
    vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);// 一个swapchan 管理多个image
    swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());

    swapChainImageFormat = surfaceFormat.format;
    swapChainExtent = extent;


}


void SwapChain::createImageViews()
{
    swapChainImageViews.resize(swapChainImages.size());

    for (size_t i = 0; i < swapChainImages.size(); i++)
    {
        swapChainImageViews[i] = Texture::CreateImageView(swapChainImages[i], swapChainImageFormat);

    }
}

void SwapChain::createDepthResources()
{


}

void SwapChain::createRenderPass()
{
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = swapChainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;//多重采样的采样数，1表示不使用多重采样
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;//在渲染开始时清除附件的内容，通常会将画布清空为一个指定的颜色。
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;//在渲染结束时将附件的内容存储到内存中，以便后续使用或者显示。
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;//在渲染开始时不关心模板附件的内容，通常用于不使用模板测试的情况。
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;//在渲染结束时不关心模板附件的内容，通常用于不使用模板测试的情况。
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;//在渲染开始时不关心附件的初始布局，通常会将画布的布局设置为未定义，以便GPU可以根据需要进行优化。
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;//在渲染结束时将附件的布局转换为适合显示的布局，以便将渲染结果呈现到屏幕上。

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;//图形管线还是计算管线
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;//颜色附件引用，指定了子通道使用哪个附件作为颜色输出，以及该附件的布局。

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;//RenderPass 开始前的操作
    dependency.dstSubpass = 0;//RenderPass 结束后的操作
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;//等待源执行到颜色附件输出阶段
    dependency.srcAccessMask = 0;//不需要等待任何特定的访问权限，因为我们只关心阶段的完成。
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;//使目标停留在颜色附件输出阶段
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;//等待目标阶段完成对颜色附件的写入操作，以确保在RenderPass结束后，渲染结果已经正确地写入到附件中。

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    Utils::vk_check(vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass));



}

void SwapChain::createFramebuffers()
{
    swapChainFramebuffers.resize(swapChainImageViews.size());

    for (size_t i = 0; i < swapChainImageViews.size(); i++)
    {
        VkImageView attachments[] = {
            swapChainImageViews[i]
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = swapChainExtent.width;
        framebufferInfo.height = swapChainExtent.height;
        framebufferInfo.layers = 1;

        Utils::vk_check(vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]));

    }
}

void SwapChain::createSyncObjects()
{
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {

        Utils::vk_check(vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]));
        Utils::vk_check(vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]));
        Utils::vk_check(vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]));
    }

}

void SwapChain::recreateSwapchain()
{

    glm::vec2 wh = glWindow::getInstance().getFrameSize();
    while (wh.x == 0 || wh.y == 0) {
        wh = glWindow::getInstance().getFrameSize();
        glfwWaitEvents();

    }
    vkDeviceWaitIdle(device);
    cleanUpSwapchain();
    createSwapChain();
    createImageViews();
    createFramebuffers();

}

void SwapChain::cleanUpSwapchain() {
    for (auto imageView : swapChainImageViews) {
        vkDestroyImageView(device, imageView, nullptr);
    }
    swapChainImageViews.clear();

    if (swapChain != nullptr) {
        vkDestroySwapchainKHR(device, swapChain, nullptr);
        swapChain = nullptr;
    }

    for (auto framebuffer : swapChainFramebuffers) {
        vkDestroyFramebuffer(device, framebuffer, nullptr);
    }
}

void SwapChain::clean()
{

    cleanUpSwapchain();

    vkDestroyRenderPass(device, renderPass, nullptr);
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
        vkDestroyFence(device, inFlightFences[i], nullptr);
    }

}

SwapChainSupportDetails SwapChain::querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface) 
{
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
    }

    return details;
}
VkPresentModeKHR SwapChain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}
VkExtent2D SwapChain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window) {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    }
    else {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        VkExtent2D actualExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}
VkSurfaceFormatKHR SwapChain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

VkResult SwapChain::acquireNextImage(uint32_t* imageIndex)
{
    vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);//CPU等待GPU  完成当期帧渲染 通过等待fence
    return vkAcquireNextImageKHR(device,swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, imageIndex); //   获取下一张画布，但是这张画布要等 imageAvailableSemaphore才能写入东西

}

VkResult SwapChain::submitCmd(VkCommandBuffer* pCmd, uint32_t cmdCount,uint32_t* imageIndex)
{
    VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
    VkSubmitInfo submitInfo{}; // 
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = cmdCount;
    submitInfo.pCommandBuffers = pCmd;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;
    vkResetFences(device, 1, &inFlightFences[currentFrame]); // 将fence设置成等待状态
    if (vkQueueSubmit(Device::getInstance().graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS)
    {
        //等待imageAvailableSemaphore画布可画,将画布提交给GPU处理，
        //处理完成后产生一个完成信号 renderFinishedSemaphore，将fence设置成完成状态
        throw std::runtime_error("failed to submit draw command buffer!");
    }
    VkSwapchainKHR swapChains[] = { swapChain };
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores; //等待GPU完成信号 renderFinishedSemaphore ，表示完整的画面处理完，否则可能会出现处理了一半的画面。
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = imageIndex;

    VkResult result = vkQueuePresentKHR(Device::getInstance().presentQueue, &presentInfo); //GPU 将画面显示到平屏幕
    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    return result;
}
