#pragma once
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_LEFT_HANDED
#define GLFW_INCLUDE_VULKAN
#define GLM_FORCE_RADIANS
#include <vector>
#include <GLFW/glfw3.h>
#include <optional>
#include <iostream>
#include <fstream>
#include <set>
#include <algorithm>
#include <array>
#include "render/vk_mem_alloc.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>
#include <functional>
#include "input/Input.h"
#include <thread>
#include "render/Texture.h"
#include "render/IndexBuffer.h"
#include "render/VertexBuffer.h"
#include "render/CommandBuffer.h"
#include "render/UniformBuffer.h"
#include "render/Pipeline.h"
#include "render/Shader.h"
#include "render/SwapChain.h"
#include "render/DescriptorSetManager.h"
#include "render/Sampler.h"
#include "render/camera/Camera.h"
#include "window/glWindow.h"
#include "render/VObject.h"
#include "imgui/imguiINI.h"
#include "render/PolygonRender.h"
#include "render/SpriteRender.h"
#include "render/Animation.h"
#include "box2d/box2d.h"
#include "physic/debug/DebugDraw.h"
#include "render/Particle.h"
//sheet 分割
//像素
//矩形框/
//uv框
class VulkanEngine
{
protected:
     float width = WINDOW_WIDTH, height = WINDOW_HEIGHT;
     float lastTime = 0;
     GLFWwindow* hwindow = 0;
     VkDevice  device;
     SwapChain swapchain;
     Camera camera;
     UniformBuffer ubs[MAX_FRAMES_IN_FLIGHT];
     CommandBuffer cbs[MAX_FRAMES_IN_FLIGHT];
     VkDescriptorSet sets[MAX_FRAMES_IN_FLIGHT];
     DSetLayout setLayout;
     float dt = 0.05;
     volatile bool destroy = false; 
public :
    void Init();
    virtual ~VulkanEngine() {};
    void RecordCommandBuffer(CommandBuffer& commandBuffer, uint32_t imageIndex);
    virtual void UserInit() {};
    virtual void UserUpdate(CommandBuffer& cmd,int currentFrame) {};
    virtual void UserClean() {};
    void createDescriptorSets();
    void createBuffers();
    void Render();
    void MessageLoop();
    void CleanUp();
    void Run() {
        MessageLoop();
    }

};

