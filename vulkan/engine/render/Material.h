#pragma once
#include "Texture.h"
#include "Shader.h"
#include "glm/glm.hpp"
#include "Pipeline.h"
#include "UniformBuffer.h"



class Material
{
    
    glm::vec4 baseColor; //颜色
    Texture* baseTex = 0; //基础贴图
   // float metal = 0; //
   // float smooth = 0; //
    
public:
    Pipeline* pipline = 0;//使用哪个管线来渲染这个材质
    void init();
    void setPipeline(Pipeline* pipeline);
    // 添加纹理资源
    void setTexture(Texture* texture);

    // 绑定材质到渲染命令缓冲区
    void bind(CommandBuffer& cmdBuffer,uint32_t currentFrame);

    // 更新描述符集（每帧调用）
    void updateDescriptorSets();

    void createDescriptorSets();

    // 描述符相关
    std::vector<VkDescriptorSetLayout> m_setLayouts;// 
    // 描述符集
    std::vector<VkDescriptorSet> m_sets[MAX_FRAMES_IN_FLIGHT]; //set0 set1 set2 
    void clean();
    ~Material() {
        clean();
    }
};

