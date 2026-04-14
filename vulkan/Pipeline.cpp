#include "Pipeline.h"

void Pipeline::CreateLayout(VkDescriptorSetLayout* layouts, int layoutCount, VkPushConstantRange* ranges, int rangeCount)
{


    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = layoutCount;
    pipelineLayoutInfo.pSetLayouts = layouts;
    pipelineLayoutInfo.pushConstantRangeCount = rangeCount;
    pipelineLayoutInfo.pPushConstantRanges = ranges;

    if (vkCreatePipelineLayout(Device::Instance().device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }
}

void Pipeline::Create(PipelineConfig& config)
{
    vert = config.vertShader;
    frag = config.fragShader;
    CreateLayout(
        config.setLayouts.data(),
        config.setLayouts.size(),
        config.pushConstants.data(),
        config.pushConstants.size()
    );//´´½¨pipeline layout 

    VkPipelineShaderStageCreateInfo shaderStages[2]{};
    shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    shaderStages[0].module = config.vertShader;
    shaderStages[0].pName = "main";


    shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    shaderStages[1].module = config.fragShader;
    shaderStages[1].pName = "main";


    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &config.vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &config.inputAssembly;
    pipelineInfo.pViewportState = &config.viewportState;
    pipelineInfo.pRasterizationState = &config.rasterizer;
    pipelineInfo.pMultisampleState = &config.multisampling;
    pipelineInfo.pDepthStencilState = &config.depthStencil;
    pipelineInfo.pColorBlendState = &config.colorBlending;
    pipelineInfo.pDynamicState = &config.dynamicState;


    pipelineInfo.layout = pipelineLayout;
    pipelineInfo.renderPass = config.renderPass;
    pipelineInfo.subpass = 0;


    if (vkCreateGraphicsPipelines(
        Device::Instance().device,
        VK_NULL_HANDLE,
        1,
        &pipelineInfo,
        nullptr,
        &pipeline) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create graphics pipeline!");
    }
}

void Pipeline::Clean()
{
    vkDestroyShaderModule(Device::Instance().device,frag,nullptr);
    vkDestroyShaderModule(Device::Instance().device,vert,nullptr);
    vkDestroyPipeline(Device::Instance().device, pipeline,nullptr);
    vkDestroyPipelineLayout(Device::Instance().device, pipelineLayout, nullptr);
}
