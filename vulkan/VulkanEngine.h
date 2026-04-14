#pragma once
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_RIGHT_HANDED
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
#include "vk_mem_alloc.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>
#include <functional>
#include "Input.h"
#include <thread>
#include "DescriptorSetManager.h"
#include "Camera.h"
#include "gui/gui.h"
#include "Image.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "CommandBuffer.h"
#include "UniformBuffer.h"
#include "Pipeline.h"
#include "Global.h"
#include "Shader.h"
#include "GridRender.h"
#include "Sprite.h"
#include "SwapChain.h"
#include "glWindow.h"
#include "Device.h"
#define MAX_FRAMES_IN_FLIGHT  2

//struct FrameResource
//{
//    UniformBuffer mvp;
//    UniformBuffer info;
//    VkDescriptorSet set0;//有好几个set
//    CommandBuffer cmd;//当前使用的
//    static VkDescriptorSetLayout layoutSet;
//
//    virtual void init()
//    {   
//        if (layoutSet == 0)
//        {
//            DescriptorSetLayoutBuilder builder; // 描述的是set 和 binding
//            layoutSet = builder.SetDevice(Device::Instance().device)
//                .AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 1)
//                .AddBinding(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL, 1)
//                .build();
//        }
//        mvp.AllocBuffer(sizeof(MVP));
//        info.AllocBuffer(sizeof(DataInfo));
//        DescriptorSetPool::Instance().AllocSets(1, &layoutSet, &set0);
//        WriteSetHelper builder;
//        VkDescriptorBufferInfo mvpBufInfo;
//        mvpBufInfo.buffer = mvp.GetBuffer();
//        mvpBufInfo.offset = 0;
//        mvpBufInfo.range = sizeof(MVP);////
//        VkDescriptorBufferInfo infoBuf;
//        mvpBufInfo.buffer = info.GetBuffer();
//        mvpBufInfo.offset = 0;
//        mvpBufInfo.range = sizeof(DataInfo);////
//        builder
//            .AddWriteBuffer(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,set0, &mvpBufInfo)
//            .AddWriteBuffer(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,set0, &infoBuf)
//            .Update();
//        cmd.Create();
//    }
//    void update(MVP& m, DataInfo& o)
//    {
//        mvp.CopyUniformData((char*) & m);
//        info.CopyUniformData((char*)& o);
//
//    }
//};
//VkDescriptorSetLayout FrameResource::layoutSet = 0;
class VulkanEngine
{
    SpriteRender spriterender;
    float width = 800, height = 600;
    float lastTime, deltaTime;
    VkDevice  device;
    SwapChain swapchain;
    Sprite sprite;
    Sprite sprite2;
    std::vector <CommandBuffer> commandBuffer ;
    VkSampler sampler;


public :
    void Init();
    VulkanEngine() 
    {

    }

    void RecordCommandBuffer(CommandBuffer commandBuffer, uint32_t imageIndex);
    //virtual void InitUser();
    //virtual void UpdataUser();
    //virtual void CleanUser();

    void createTextureSampler() {
        sampler = Image::CreateSampler();

    }


    double GetTimeSeconds()
    {
        static auto start = std::chrono::steady_clock::now();
        return std::chrono::duration<float>(
            std::chrono::steady_clock::now() - start
        ).count();
    }
    void createCmds() {
        commandBuffer.resize(MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
            commandBuffer[i].Create();
    }

    void Render(float dt);
    void MainLoop();
    void CleanUp();
    void Run() {
        Init();
        MainLoop();
        CleanUp();
    }

};

