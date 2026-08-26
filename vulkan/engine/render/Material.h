#pragma once
#include "Texture.h"
#include "Shader.h"
#include "glm/glm.hpp"
#include "Pipeline.h"
#include "UniformBuffer.h"
#include "DescriptorSetManager.h"


class Material
{
    
    glm::vec4 baseColor; //颜色
    Texture baseTex; //基础贴图

public:
    Pipeline* pipline = 0;//使用哪个管线来渲染这个材质
    void init();
    void setPipeline(Pipeline* pipeline);
    // 添加纹理资源

    // 绑定材质到渲染命令缓冲区
    void bind(CommandBuffer& cmdBuffer,uint32_t currentFrame);
    // 更新描述符集（每帧调用）
    void clean();
    ~Material() {
        clean();
    }
};

