#pragma once
#include "Image.h"
#include "VertexBuffer.h"
#include "UniformBuffer.h"
#include "Pipeline.h"
#include "CommandBuffer.h"
#include "DescriptorSetManager.h"
#include <glm/gtc/matrix_transform.hpp>
#include <array>
#include "Camera2D.h"
struct Transform2D 
{
    glm::vec2 position; //屏幕坐标
    glm::vec2 scale;
    float rotation;//角度
    glm::vec2 pivot;//轴心位置
    void SetPivot(glm::vec2 p) 
    {
        pivot = p;
    }
    void Rotate(float angle) 
    {
        rotation += angle;
    }

    glm::mat4 GetModel()
    {
        glm::mat4 model(1.0f);
        model = glm::translate(model, glm::vec3(position, 0.0f));
        model = glm::rotate(model,glm::radians(rotation), glm::vec3(0, 0, 1));
        model = glm::scale(model, glm::vec3(scale, 1.0f));
        return  model;
    }

    Transform2D() 
    {
        scale = { 1,1 };
        rotation = 0;
        position = { 0,0 };
        pivot = { 0,0 };
    }
};
struct PushData 
{
    glm::vec2 viewport;
    int index;
};

class Sprite {
public:
    float width, height;
    Transform2D trans2d;
    VkSampler sampler;
    Image img;
    int index;
    void Clean();
    VkDescriptorImageInfo getDescriptorInfo() 
    {
        VkDescriptorImageInfo info;
        info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        info.imageView = img.GetImageView();
        info.sampler = sampler;
        return info;
    }
    void LoadRes(std::string path)
    {
        img.LoadRes(path);
        img.CreateImageView(VK_FORMAT_R8G8B8A8_SRGB);
        width = img.allocImg.width;
        height = img.allocImg.height;
    }
    void SetSampler(VkSampler sap) 
    {
        sampler = sap;
    }
    Sprite()
     :width(0),height(0),index(0),sampler(0)
    {

    }
    Sprite(std::string path)
    {
        LoadRes(path);
    }
    ~Sprite() 
    {

    }
};
struct SpriteVertex
    {
        glm::vec2 pos; ////
        glm::vec2 uv;  ////
        static VkVertexInputBindingDescription getBindingDescription()
        {
            VkVertexInputBindingDescription bindingDescription{};
            bindingDescription.binding = 0;
            bindingDescription.stride = sizeof(SpriteVertex);
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
            return bindingDescription;
        }

        static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() 
        {
            std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
            attributeDescriptions[0].binding = 0;
            attributeDescriptions[0].location = 0;
            attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
            attributeDescriptions[0].offset = offsetof(SpriteVertex, pos);

            attributeDescriptions[1].binding = 0;
            attributeDescriptions[1].location = 1;
            attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
            attributeDescriptions[1].offset = offsetof(SpriteVertex, uv);
            return attributeDescriptions;
        }
    };

class SpriteRender
{
    
    Pipeline pipeline;//绘制图片的pipeline
    VertexBuffer vb[MAX_FRAMES_IN_FLIGHT];///
    UniformBuffer ub[MAX_FRAMES_IN_FLIGHT];
    VkDescriptorSetLayout setLayout;
    VkDescriptorSet set[MAX_FRAMES_IN_FLIGHT];
    std::vector<Sprite*> spts;

 public:
    void init(VkRenderPass renderPass);
    void Draw(CommandBuffer cmd, Sprite& sp, int currentFrame);
    void AddSprite(Sprite* spt);
    void DrawSprites(CommandBuffer cmd, int currentFrame);
    void Clean();
};

