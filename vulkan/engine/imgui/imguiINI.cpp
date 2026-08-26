#include "imguiINI.h"
#include "../utils/Utils.h"
#include "GLFW/glfw3.h"
void Imgui_init(
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
void Imgui_clean()
{
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}