#define VMA_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "EngineTestMain.h"
#include <math.h>
#include "Global.h"

struct VP {
    glm::mat4 v{};
    glm::mat4 p{};
};
extern VkDescriptorSet g_set0[];
extern VkDescriptorSetLayout g_layout;
extern VkRenderPass g_renderPass;
VP mats;

void VulkanEngine::Init()
{

 
    glWindow::getInstance().createWindow(this->width, this->height);
    Device::getInstance().init();
    device = Device::getInstance().device;
    Allocator::init();
    PoolManager::inst().init();
    CommandBufferPool::init();
    swapchain.init();
    g_renderPass = swapchain.renderPass;
    SamplerManager::createDefaultSamplers();
 
    createBuffers();
    createDescriptorSets();


    camera.setOrthographic(
        0,
        glWindow::getInstance().width / (PIXEL_PER_UNIT),
        0,
        glWindow::getInstance().height / (PIXEL_PER_UNIT),
        -1000.0f,
        1000.0f);


    UserInit();

   

}



void VulkanEngine::RecordCommandBuffer(CommandBuffer& cmd, uint32_t imageIndex ) {
   

    int currentFrame = swapchain.currentFrame;
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    cmd.begain();

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = swapchain.renderPass;
    renderPassInfo.framebuffer = swapchain.getFrameBuffer(imageIndex);
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = swapchain.getExtent();

    VkClearValue clearColor = { {{.0f, .0f, .0f, 1.0f}} };//{ {{1.0f, 0.75f, 0.8f, 1.0f}} };
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;


    cmd.beginRenderPass(renderPassInfo);

    //设置视口
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(swapchain.getExtent().width);
    viewport.height = static_cast<float>(swapchain.getExtent().height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    cmd.setViewport(&viewport);


    //设置裁剪区域
    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = swapchain.getExtent();
    cmd.setScissor(&scissor);

    camera.updateData(ubs[currentFrame]);

    UserUpdate(cmd,currentFrame);

    cmd.endRenderPass();
    cmd.end();

}



void VulkanEngine::createDescriptorSets()
{
   
    DescriptorSetLayoutBuilder builder1;
    setLayout = builder1
        .AddBinding(0, 
            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 
            VK_SHADER_STAGE_VERTEX_BIT| VK_SHADER_STAGE_FRAGMENT_BIT, 1)
        .build();
    g_layout = setLayout.layout;
   
    std::vector<DSetLayout*> setLayouts;

    DoTimes([&](int i) {
        setLayouts.push_back(&setLayout);
        });

    PoolManager::inst().allocSets(setLayouts.data(), setLayouts.size(),sets);

    DoTimes([&](int i) {
        g_set0[i] = sets[i];
        });
   
    DoTimes([&](int i) {
        WriteSetHelper helper;
        VkDescriptorBufferInfo bf = ubs[i].getDescriptorInfo();
        helper
            .AddWriteBuffer(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, sets[i], &bf)
            .Update();
        });

}


void VulkanEngine::createBuffers()
{

     for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
     {
         cbs[i].create();
         ubs[i].allocBuffer(sizeof(VP));
       
     }

}


void VulkanEngine::Render()
{
    while (!this->destroy)
    {
        float currentTime = glfwGetTime();
        dt = currentTime - lastTime;
        lastTime = currentTime;
        Input::Keyboard::Update(this->hwindow);
        Input::Mouse::Update(this->hwindow);
        //双缓冲 可能是大于2的
        uint32_t imageIndex;
        VkResult result = swapchain.acquireNextImage(&imageIndex); //获取下一张画布，但是这张画布要等 imageAvailableSemaphore才能写入东西

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            swapchain.recreateSwapchain();
            return;
        }
        else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////
        RecordCommandBuffer(cbs[swapchain.currentFrame], imageIndex);// 重新绘制
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        result = swapchain.submitCmd(&cbs[swapchain.currentFrame].m_command, 1, &imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || glWindow::getInstance().wasWindowResized())
        {
            glWindow::getInstance().resetWindowResizedFlag();
            swapchain.recreateSwapchain();
        }
        else if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to present swap chain image!");
        }
    }


   vkDeviceWaitIdle(device);
}


void VulkanEngine::MessageLoop()
{

    auto& window = glWindow::getInstance();
    this->hwindow = window.getGLFWwindow();
    std::thread th(&VulkanEngine::Render, this);
    while (!window.shouldClose())
    {
        glfwPollEvents();
    
    }
    this->destroy = true;
    th.join();

}
//有两个线程，一个线程往写int，另一个线程读取int。

void VulkanEngine::CleanUp()
{
   

    UserClean();
    swapchain.clean();
    vkDestroyDescriptorSetLayout(device, setLayout.layout,0);


}
