#pragma once
#include "vulkan/vulkan.h"
#include "Shader.h"
#include "Device.h"
struct PipelineConfig {

    VkRenderPass renderPass = 0;
    std::vector<VkDescriptorSetLayout> setLayouts;
    std::vector<VkPushConstantRange> pushConstants;

    VkShaderModule vertShader =0;
    VkShaderModule fragShader =0;
    
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    VkPipelineViewportStateCreateInfo viewportState{};
    VkPipelineRasterizationStateCreateInfo rasterizer{};
    VkPipelineMultisampleStateCreateInfo multisampling{};
    VkPipelineColorBlendAttachmentState blend{};
    VkPipelineColorBlendStateCreateInfo colorBlending{};
    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    VkPipelineDynamicStateCreateInfo dynamicState{};
    std::vector<VkDynamicState> dyState;
    static PipelineConfig basic() {
        PipelineConfig config;
        
        //顶点信息
        VkPipelineVertexInputStateCreateInfo* vertexInputInfo = &config.vertexInputInfo;
        vertexInputInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo->vertexBindingDescriptionCount = 0;
        vertexInputInfo->vertexAttributeDescriptionCount = 0;
        vertexInputInfo->pVertexBindingDescriptions = 0;  ///这里对应的是shader的布局
        vertexInputInfo->pVertexAttributeDescriptions = 0;
        //组装方式：直线还是三角面
        VkPipelineInputAssemblyStateCreateInfo* inputAssembly = &config.inputAssembly;
        inputAssembly->sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly->topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;//这里控制图元
        inputAssembly->primitiveRestartEnable = VK_FALSE;
        inputAssembly->pNext = NULL;
        //视口与裁剪
        VkPipelineViewportStateCreateInfo* viewportState = &config.viewportState;
        viewportState->sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState->viewportCount = 1; //规定映射的窗口区域
        viewportState->scissorCount = 1; // 裁剪窗口外的像素
        //光栅化
        VkPipelineRasterizationStateCreateInfo* rasterizer = &config.rasterizer;
        rasterizer->sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer->depthClampEnable = VK_FALSE;
        rasterizer->rasterizerDiscardEnable = VK_FALSE;     
        rasterizer->polygonMode = VK_POLYGON_MODE_FILL;  //填充整个面，还可以是线框或者点模式
        rasterizer->lineWidth = 1.0f;                    //线宽
        rasterizer->cullMode = VK_CULL_MODE_BACK_BIT;    //剔除反面
        rasterizer->frontFace = VK_FRONT_FACE_CLOCKWISE; //顺时针
        rasterizer->depthBiasEnable = VK_FALSE;          //深度
        //
        VkPipelineMultisampleStateCreateInfo* multisampling = &config.multisampling;
        multisampling->sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling->sampleShadingEnable = VK_FALSE;
        multisampling->rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisampling->minSampleShading = 1.0f; // Optional 
        multisampling->pSampleMask = nullptr; // Optional 
        multisampling->alphaToCoverageEnable = VK_FALSE; // Optional 
        multisampling->alphaToOneEnable = VK_FALSE; // Optional




        VkPipelineColorBlendAttachmentState* blend = &config.blend;
        blend->colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        blend->blendEnable = VK_TRUE;
        //颜色混合
        blend->srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        blend->dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        blend->colorBlendOp = VK_BLEND_OP_ADD;

        //透明度混合
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


        VkPipelineDepthStencilStateCreateInfo* pdepth = &config.depthStencil;
        pdepth->sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        // 1. 开启深度测试
        pdepth->depthTestEnable = VK_TRUE;
        // 2. 开启深度写入（通常情况下你也需要这个）
        pdepth->depthWriteEnable = VK_TRUE;
        // 3. 设置深度比较函数（默认使用 Less 或 LessOrEqual）
        pdepth->depthCompareOp = VK_COMPARE_OP_LESS;
        // 4. 其他不常用的配置可以关闭
        pdepth->depthBoundsTestEnable = VK_FALSE;
        pdepth->stencilTestEnable = VK_FALSE;
        // 设置 depth bounds 的默认值（如果不使用该功能）
        pdepth->minDepthBounds = 0.0f;
        pdepth->maxDepthBounds = 1.0f;
        // Stencil 操作的默认值（如果不使用 stencil）
        pdepth->front = {};
        pdepth->back = {};


        //动态属性，指定pipeline 某些属性可以被修改。增加pipeline 的灵活性。

        config.dyState.push_back( VK_DYNAMIC_STATE_VIEWPORT);
        config.dyState.push_back( VK_DYNAMIC_STATE_SCISSOR);
        config.dyState.push_back( VK_DYNAMIC_STATE_LINE_WIDTH);

        //
        VkPipelineDynamicStateCreateInfo* dynamicState = &config.dynamicState;
        dynamicState->sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState->dynamicStateCount = 3;
        dynamicState->pDynamicStates = &(config.dyState[0]);
      

        return config;


    }
    ~PipelineConfig()
    {
        if(fragShader)
            vkDestroyShaderModule(Device::getInstance().device, fragShader, 0);
        if(vertShader)
            vkDestroyShaderModule(Device::getInstance().device, vertShader, 0);
    }

};

/// <summary>
/// pipeline 就是本质的流水线。本质都是对图元数据的组装过程。
/// </summary>
class Pipeline
{

    VkPipelineLayout pipelineLayout =0;
    VkPipeline pipeline = 0;
public:
    VkPipeline get()
    {
        return pipeline;
    }
   
    VkPipelineLayout getLayout() {

        return pipelineLayout;
    }

    void createPipelineLayout(VkDescriptorSetLayout* layouts, int layoutCount, VkPushConstantRange* ranges = 0, int rangeCount = 0);
    void create(PipelineConfig& config);
    void clean();
    ~Pipeline()
    {
        clean();
    }
};

