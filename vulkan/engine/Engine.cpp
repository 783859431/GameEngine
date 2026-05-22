#define VMA_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "Engine.h"
#include <math.h>
static void Imgui_init(
    GLFWwindow* window,
    VkInstance instance,
    VkPhysicalDevice PhysicalDevice,
    VkDevice device,
    uint32_t QueueFamily,
    VkQueue q,
    VkPipelineCache PipelineCache,
    VkDescriptorPool DescriptorPool,
    uint32_t MinImageCount,
    uint32_t ImageCount,
    VkRenderPass RenderPass
)
{
    // console = new Console();
    IMGUI_CHECKVERSION();
    float main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor());
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.Fonts->AddFontFromFileTTF(
        "C:/Windows/Fonts/msyh.ttc",
        18.0f,
        nullptr,
        io.Fonts->GetGlyphRangesChineseSimplifiedCommon()
    );
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup scaling
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    style.FontScaleDpi = main_scale;        // Set initial font scale. (using io.ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForVulkan(window, true);
    ImGui_ImplVulkan_InitInfo init_info = {};
    //init_info.ApiVersion = VK_API_VERSION_1_3;              // Pass in your value of VkApplicationInfo::apiVersion, otherwise will default to header version.
    init_info.Instance = instance;
    init_info.PhysicalDevice = PhysicalDevice;
    init_info.Device = device;
    init_info.QueueFamily = QueueFamily;
    init_info.Queue = q;
    init_info.PipelineCache = PipelineCache;
    init_info.DescriptorPool = DescriptorPool;
    init_info.MinImageCount = MinImageCount;
    init_info.ImageCount = ImageCount;
    init_info.Allocator = nullptr;
    init_info.PipelineInfoMain.RenderPass = RenderPass;
    init_info.PipelineInfoMain.Subpass = 0;
    init_info.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    init_info.CheckVkResultFn = Utils::vk_check;
    ImGui_ImplVulkan_Init(&init_info);
}
struct VP {
    glm::mat4 v{};
    glm::mat4 p{};
};

VP mats;
void VulkanEngine::Init()
{
    
    glWindow::getInstance().createWindow(this->width, this->height);
    Device::getInstance().init();
    device = Device::getInstance().device;
    Allocator::init();
    DescriptorSetPool::init();
    CommandBufferPool::init();
    swapchain.init();
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = { 0,9 };
    m_worldId = b2CreateWorld(&worldDef);
    //circle.createCircle(100,400,300,3);
    //sph.mesh.createSphere(100, 16, 32);
    sph.mesh.createQuad(20, 20);
    //sph.mesh.createFromFile("model/cube.obj");
   // sph.transform.setScale(100.0f,100.0f,100.0f);
    obj.mesh.createQuad(200, 200);
    createBuffers();
    tex.loadRes("texture/container2.png");
    bochi.loadRes("texture/bocchi.png");

    createDescriptorSets();

    b2BodyDef bodyDef = b2DefaultBodyDef();
    b2ShapeDef shapeDef = b2DefaultShapeDef();

    b2Polygon box = b2MakeBox(500,100);
    bodyDef.position = { 0, 800 };
    m_bodyIds[1] = b2CreateBody(m_worldId, &bodyDef);
    b2CreatePolygonShape(m_bodyIds[1], &shapeDef, &box);

    box = b2MakeBox(100, 400);
    bodyDef.type =  b2_dynamicBody;
    bodyDef.position = { obj.transform.getPosition().x, obj.transform.getPosition().y };
    m_bodyIds[0] = b2CreateBody(m_worldId, &bodyDef);
    b2CreatePolygonShape(m_bodyIds[0], &shapeDef, &box);
    


    PipelineConfig cf = PipelineConfig::basic();
    auto attr = Vertex::getAttributeDescriptions();
    auto binding = Vertex::getBindingDescription();
    cf.vertexInputInfo.vertexAttributeDescriptionCount = attr.size();
    cf.vertexInputInfo.vertexBindingDescriptionCount = binding.size();
    cf.vertexInputInfo.pVertexAttributeDescriptions = attr.data();
    cf.vertexInputInfo.pVertexBindingDescriptions = binding.data();


    VkPushConstantRange range;
    range.offset = 0;
    range.size = sizeof(ConstData);
    range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    cf.pushConstants.push_back(range);


    cf.renderPass = swapchain.renderPass;
    cf.setLayouts.push_back(setLayout);
    cf.fragShader = Shader::LoadShader("shader/basicFrag.spv");
    cf.vertShader = Shader::LoadShader("shader/basicVert.spv");
    pipeline.create(cf);

    sph.transform.setPosition(400.0f, 300.0f, 10.0f);
   
    //bodyDef.position = { sph.transform.getPosition().x, sph.transform.getPosition().y };
    //bodyDef.rotation = { 0.0f,0.0f };
    //m_bodyIds[1] = b2CreateBody(m_worldId, &bodyDef);
    //b2CreatePolygonShape(m_bodyIds[1], &shapeDef, &box);
    sapMat.init();
    sapMat.setTexture(&tex);
    sapMat.setPipeline(&matPipeline);
    obj.material = &this->sapMat;
    sph.material = &this->sapMat;
    PipelineConfig cf2 = cf;
    cf2.setLayouts.push_back(obj.material->m_setLayouts[0]);
    cf2.fragShader = Shader::LoadShader("shader/sampleFrag.spv");
    cf2.vertShader = Shader::LoadShader("shader/sampleVert.spv");
    matPipeline.create(cf2);

    //camera.setOrthographic(0,glWindow::getInstance().width,0, glWindow::getInstance().height,-1000.0f,1000.0f);
  float aspect = (float)glWindow::getInstance().width / glWindow::getInstance().height;
    camera.setPerspective(60.0f, aspect, 0.1f, 1000.0f);
    camera.setPosition({ 400.0f,300.0f,-500.0f });
    camera.lookAt({400.0f, 300.0f, 0.0f});

  //  camera.setPerspective(60,aspect,0.1,1000);    
    //camera.setPosition({ 400,300,-5 });
    //camera.lookAt({400,300,0});
    Imgui_init(
        glWindow::getInstance().getGLFWwindow(),
        Device::getInstance().ins,
        Device::getInstance().gpu,
        Device::getInstance().device,
        Device::getInstance().indices.graphicsFamily.value(),
        Device::getInstance().graphicsQueue,
        NULL,
        DescriptorSetPool::setsPool,
        swapchain.getImageCount(),
        swapchain.getImageCount(),
        swapchain.renderPass);

}


VulkanEngine::~VulkanEngine()
{
    CleanUp();
}
float t = 0;
void VulkanEngine::changeTransform()
{
    //t += dt;
    //float a = 100 * sinf(3 * t);
    //float b = 100 * cosf(3 * t);
    b2Vec2 v = b2Body_GetPosition(m_bodyIds[0]);
    obj.transform.setPosition({v.x,v.y,0.0f});
}

void VulkanEngine::RecordCommandBuffer(CommandBuffer& cmd, uint32_t imageIndex ) {
    b2World_Step(m_worldId,1/60.0f,4);
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

    VkClearValue clearColor = { {{1.0f, 0.75f, 0.8f, 1.0f}} };
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;
    changeTransform();

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
    handleKey();
    //b2Polygon box = b2MakeOffsetBox(100.0f, 100.0f, {400.0f,300.0f}, {0,0});
    //auto proxy = b2MakeProxy(box.vertices, box.count, box.radius);
    //b2World_OverlapShape(m_worldId, &proxy, b2DefaultQueryFilter(), OverlapResultFcn, this);
    camera.updateData(ubs[currentFrame]);

    cmd.bindSets(pipeline.getLayout(), &sets[currentFrame], 1, 0);
    sph.draw(cmd, currentFrame);
    obj.draw(cmd,currentFrame);
    DrawGUI(cmd);





    cmd.endRenderPass();
    cmd.end();

}

void VulkanEngine::createDescriptorSets()
{
   

    DescriptorSetLayoutBuilder builder1;
    setLayout = builder1
        .AddBinding(0, 
            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 
            VK_SHADER_STAGE_VERTEX_BIT, 1)
        .build();
    //DescriptorSetLayoutBuilder builder2;
   /* PbrLayout = builder2
        .AddBinding(
            0,
            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            VK_SHADER_STAGE_FRAGMENT_BIT,
            1)
        .AddBinding(
            1, 
            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            VK_SHADER_STAGE_FRAGMENT_BIT,
            1)
        .build();*/
    VkDescriptorSetLayout layouts[MAX_FRAMES_IN_FLIGHT];
    //VkDescriptorSetLayout layouts2[MAX_FRAMES_IN_FLIGHT];
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
         layouts[i] = setLayout;
         //layouts2[i] = PbrLayout;
    }
       
    DescriptorSetPool::allocSets(MAX_FRAMES_IN_FLIGHT, &layouts[0], &sets[0]);
    //DescriptorSetPool::allocSets(MAX_FRAMES_IN_FLIGHT, &layouts2[0], &sets1[0]);
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        WriteSetHelper helper;
        VkDescriptorBufferInfo bf = ubs[i].getDescriptorInfo();
        helper
            .AddWriteBuffer(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, sets[i], &bf)
            .Update();


        //WriteSetHelper helper2;
        //auto imgInfo = tex.getDescriptorImageInfo();
        //helper2
        //    .AddWriteImage(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, sets1[i], &imgInfo, 1)
        //    .Update();
    }
}

void VulkanEngine::DrawGUI(CommandBuffer& cmd)
{
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();


    {   ImGui::NewFrame();
        ImGui::Begin("Camera");                                 // Create a window called "Hello, world!" and append into it.
        ImGui::SliderFloat("x", &camera.m_position.x, -500.0f, 500.0f);
        ImGui::SliderFloat("y", &camera.m_position.y, -500.0f, 500.0f);
        ImGui::SliderFloat("z", &camera.m_position.z, -500.0f, 500.0f);

        ImGui::End();  
        ImGui::EndFrame();  
    }

    // console->Draw("console");
    ImGui::Render();
    ImDrawData* draw_data = ImGui::GetDrawData();
    const bool is_minimized = (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f);
    ImGui_ImplVulkan_RenderDrawData(draw_data, cmd.m_command);

}

void VulkanEngine::createBuffers()
{

     for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
     {
         cbs[i].create();
         ubs[i].allocBuffer(sizeof(VP));
       
     }

}

float speed = 100.0f;

void VulkanEngine::handleKey()
{
    float delta = dt * speed;
    if (Input::Keyboard::KeyHold(GLFW_KEY_A))
    {
        camera.translate({ -delta,0.0,0.0 });
    }
    else if (Input::Keyboard::KeyHold(GLFW_KEY_D)) {
        camera.translate({ delta,0.0,0.0 });
    }
    else if (Input::Keyboard::KeyHold(GLFW_KEY_W))
    {
        camera.translate({ 0.0,-delta,0.0 });
    }
    else if (Input::Keyboard::KeyHold(GLFW_KEY_S))
    {
        camera.translate({ 0.0,delta,0.0 });
    }
    camera.update();
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
    handleKey();
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    RecordCommandBuffer(cbs[swapchain.currentFrame], imageIndex);// 重新绘制
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    result =  swapchain.submitCmd(&cbs[swapchain.currentFrame].m_command,1,&imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || glWindow::getInstance().wasWindowResized())
    {
        glWindow::getInstance().resetWindowResizedFlag();
        swapchain.recreateSwapchain();
    }
    else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }


}

void VulkanEngine::MainLoop()
{
    bool first = true;
    glWindow& window = glWindow::getInstance();
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
        dt = deltaTime;
        Render(deltaTime);
        Input::Mouse::scrollY = 0;
        glfwPollEvents();
    
    }

    vkDeviceWaitIdle(device);
}

void VulkanEngine::CleanUp()
{
   // WindowGUI::clean();

    Texture::destroyDefaultSampler();
    swapchain.clean();
    vkDestroyDescriptorSetLayout(device, setLayout,0);
    //vkDestroyDescriptorSetLayout(device, setLayout2, 0);
}
