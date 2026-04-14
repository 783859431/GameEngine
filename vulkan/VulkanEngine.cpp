#define VMA_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "VulkanEngine.h"

Sprite* spt;
void VulkanEngine::Init()
{
    glWindow::Instance().init();
    Device::Instance().init();
    device = Device::Instance().device;
    Allocator::Instance().init();
    DescriptorSetPool::Instance().init();
    CommandBufferPool::Instance().init();
    swapchain.init();
    createTextureSampler();
    spriterender.init(swapchain.renderPass);
    sprite.LoadRes("textures/bocchi.png");
    sprite.SetSampler(sampler);
    spt = &sprite;
    spriterender.AddSprite(&sprite);
    createCmds();
    WindowGUI::init(
        glWindow::Instance().getGLFWwindow(),
        Device::Instance().ins,
        Device::Instance().gpu,
        Device::Instance().device,
        Device::Instance().indices.graphicsFamily.value(),
        Device::Instance().graphicsQueue,
        NULL,
        DescriptorSetPool::Instance().setsPool,
        swapchain.getImageCount(),
        swapchain.getImageCount(),
        swapchain.renderPass);

}

void VulkanEngine::RecordCommandBuffer(CommandBuffer cmd, uint32_t imageIndex ) {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    cmd.Begain();

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = swapchain.renderPass;
    renderPassInfo.framebuffer = swapchain.getFrameBuffer(imageIndex);
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = swapchain.getExtent();

    VkClearValue clearColor = { {{1.0f, 0.75f, 0.8f, 1.0f}} };
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    cmd.BeginRenderPass(renderPassInfo);
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(swapchain.getExtent().width);
    viewport.height = static_cast<float>(swapchain.getExtent().height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    cmd.SetViewport(&viewport);

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = swapchain.getExtent();
    cmd.SetScissor(&scissor);


    spriterender.DrawSprites(cmd,swapchain.currentFrame);
    WindowGUI::Draw(cmd.command);
    cmd.EndRenderPass();
    cmd.End();

}

void VulkanEngine::Render(float delta)
{

    //双缓冲 可能是大于2的
    uint32_t imageIndex;
    VkResult result = swapchain.acquireNextImage(&imageIndex); //   获取下一张画布，但是这张画布要等 imageAvailableSemaphore才能写入东西
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        swapchain.recreateSwapchain();
        return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    RecordCommandBuffer(commandBuffer[swapchain.currentFrame], imageIndex);// 重新绘制
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    result =  swapchain.submitCmd(&commandBuffer[swapchain.currentFrame].command,&imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || glWindow::Instance().wasWindowResized())
    {
        glWindow::Instance().resetWindowResizedFlag();
        swapchain.recreateSwapchain();
    }
    else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }


}

void VulkanEngine::MainLoop()
{
    bool first = true;
    glWindow& window = glWindow::Instance();
    while (!window.shouldClose())
    {
        Input::Keyboard::Update(window.getGLFWwindow());
        Input::Mouse::Update(window.getGLFWwindow());
        float currentTime = glfwGetTime();
        if (first)
        {
            lastTime = currentTime;
            first = false;
        }
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        Render(deltaTime);
        Input::Mouse::scrollY = 0;
        glfwPollEvents();
    
    }

    vkDeviceWaitIdle(device);
}

void VulkanEngine::CleanUp()
{
    WindowGUI::clean();

    CommandBufferPool::Instance().Clean();
    DescriptorSetPool::Instance().Clean();
    swapchain.clean();

    spriterender.Clean();
    sprite.Clean();
    sprite2.Clean();

    vkDestroySampler(device,sampler,NULL);
    Allocator::Instance().Clean();
    Device::Instance().Clean();
    glWindow::Instance().Clean();
}
