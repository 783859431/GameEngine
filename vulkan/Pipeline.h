#pragma once
#include "vulkan/vulkan.h"
#include "Shader.h"
#include "Device.h"
struct PipelineConfig {

    VkRenderPass renderPass;
    VkExtent2D extent;
    std::vector<VkDescriptorSetLayout> setLayouts;
    std::vector<VkPushConstantRange> pushConstants;

    VkShaderModule vertShader;
    VkShaderModule fragShader;

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    VkPipelineViewportStateCreateInfo viewportState{};
    VkPipelineRasterizationStateCreateInfo rasterizer{};
    VkPipelineMultisampleStateCreateInfo multisampling{};
    VkPipelineColorBlendAttachmentState blend{};
    VkPipelineColorBlendStateCreateInfo colorBlending{};
    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    VkPipelineDynamicStateCreateInfo dynamicState{};
    VkDynamicState dyState[2];
    static PipelineConfig basic() {
        PipelineConfig config;
        

        VkPipelineVertexInputStateCreateInfo* vertexInputInfo = &config.vertexInputInfo;
        vertexInputInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo->vertexBindingDescriptionCount = 1;
        vertexInputInfo->vertexAttributeDescriptionCount = 0;
        vertexInputInfo->pVertexBindingDescriptions = 0;  ///hwo to fix 
        vertexInputInfo->pVertexAttributeDescriptions = 0;
        /////

        VkPipelineInputAssemblyStateCreateInfo* inputAssembly = &config.inputAssembly;
        inputAssembly->sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly->topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly->primitiveRestartEnable = VK_FALSE;
        inputAssembly->pNext = NULL;

        VkPipelineViewportStateCreateInfo* viewportState = &config.viewportState;
        viewportState->sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState->viewportCount = 1; //规定映射的窗口区域
        viewportState->scissorCount = 1; // 裁剪窗口外的像素
  
        VkPipelineRasterizationStateCreateInfo* rasterizer = &config.rasterizer;
        rasterizer->sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer->depthClampEnable = VK_FALSE;
        rasterizer->rasterizerDiscardEnable = VK_FALSE;     
        rasterizer->polygonMode = VK_POLYGON_MODE_FILL;     //填充
        rasterizer->lineWidth = 1.0f;                      //线宽
        rasterizer->cullMode = VK_CULL_MODE_BACK_BIT;     // 剔除反面
        rasterizer->frontFace = VK_FRONT_FACE_CLOCKWISE; // 顺时针
        rasterizer->depthBiasEnable = VK_FALSE;          //深度

        VkPipelineMultisampleStateCreateInfo* multisampling = &config.multisampling;
        multisampling->sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling->sampleShadingEnable = VK_FALSE;
        multisampling->rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;




        VkPipelineColorBlendAttachmentState* blend = &config.blend;
        blend->colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        blend->blendEnable = VK_TRUE;
        blend->srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        blend->dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        blend->colorBlendOp = VK_BLEND_OP_ADD;

        // Alpha 混合（推荐这样）
        blend->srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        blend->dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        blend->alphaBlendOp = VK_BLEND_OP_ADD;

        VkPipelineColorBlendStateCreateInfo* colorBlending = &config.colorBlending ;
        colorBlending->sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending->logicOpEnable = VK_FALSE;
        colorBlending->logicOp = VK_LOGIC_OP_COPY;
        colorBlending->attachmentCount = 1;
        colorBlending->pAttachments = blend;
        colorBlending->blendConstants[0] = 0.0f;
        colorBlending->blendConstants[1] = 0.0f;
        colorBlending->blendConstants[2] = 0.0f;
        colorBlending->blendConstants[3] = 0.0f;


        config.dyState[0] = VK_DYNAMIC_STATE_VIEWPORT;
        config.dyState[1] = VK_DYNAMIC_STATE_SCISSOR;

        VkPipelineDynamicStateCreateInfo* dynamicState = &config.dynamicState;
        dynamicState->sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState->dynamicStateCount = 2;
        dynamicState->pDynamicStates = &(config.dyState[0]);


        return config;


    }

};

/// <summary>
/// pipeline 就是本质的流水线。本质都是对图元数据的组装过程。
/// </summary>
class Pipeline
{

    VkPipelineLayout pipelineLayout;
    VkPipeline pipeline;
    VkShaderModule vert;
    VkShaderModule frag;
public:
    VkPipeline get()
    {
        return pipeline;
    }
   
    VkPipelineLayout GetLayout() {

        return pipelineLayout;
    }

    void CreateLayout(VkDescriptorSetLayout* layouts, int layoutCount, VkPushConstantRange* ranges = 0, int rangeCount = 0);
    void Create(PipelineConfig& config);
    void Clean();
};

