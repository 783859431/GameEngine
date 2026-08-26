#pragma once
#include <vulkan/vulkan.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
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
);

void Imgui_clean();
