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
//#include "box2d/box2d.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
#include "render/PolygonRender.h"
#include "render/SpriteRender.h"
#include "render/Animation.h"
//sheet 分割
//像素
//矩形框
//uv框
class VulkanEngine
{
    float width = 800, height = 600;
    float lastTime, deltaTime;
    AnimationSprite sp_hero1;
    AnimationSprite sp_hero2;
    SpriteRender spRender; 
    VkDevice  device;
    SwapChain swapchain;
    Pipeline pipeline;
    Pipeline matPipeline;
    PolygonRender poly;
    //b2BodyId m_bodyIds[2]; 
    //b2WorldId m_worldId;
    Camera camera;
    UniformBuffer ubs[MAX_FRAMES_IN_FLIGHT];
    CommandBuffer cbs[MAX_FRAMES_IN_FLIGHT];
    VkDescriptorSet sets[MAX_FRAMES_IN_FLIGHT];
    DSetLayout setLayout;
    //VkDescriptorSetLayout PbrLayout;

    //Mesh circle;
    VObject obj;
    VObject sph;
    Texture tex;
    Texture bochi;
    Texture hero;
    Texture hero2;
    Material sapMat;
    float dt = 0;
public :
    void Init();
    void handleKey();
    VulkanEngine() 
    {

    }
    ~VulkanEngine();
    void RecordCommandBuffer(CommandBuffer& commandBuffer, uint32_t imageIndex);
    //virtual void InitUser();
    //virtual void UpdataUser();
    //virtual void CleanUser();

    void changeTransform();
    void createDescriptorSets();
 
    void DrawGUI(CommandBuffer& cmd);

    double GetTimeSeconds()
    {
        static auto start = std::chrono::steady_clock::now();
        return std::chrono::duration<float>(
            std::chrono::steady_clock::now() - start
        ).count();
    }
    void createBuffers();
    void Render(float dt);
    void MainLoop();
    void CleanUp();
    void Run() {
        Init();
        MainLoop();
    }

};

