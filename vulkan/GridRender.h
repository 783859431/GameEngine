#pragma once
#include "Pipeline.h"
#include "glm/glm.hpp"
#include "DescriptorSetManager.h"
#include "CommandBuffer.h"
#include "Input.h"
struct GridData {
    glm::vec4 pos_zoom;
    glm::vec2 viewport;
    glm::vec2 pad;
};
class GridRender
{
	Pipeline pipeline;
    VkDescriptorSetLayout setLayout;
    glm::vec2 camPos;
public :
    void draw(CommandBuffer cmd,float t);
	void init(VkRenderPass renderPass)
	{
        DescriptorSetLayoutBuilder builder;
        setLayout = builder.SetDevice(Device::Instance().device).build();

        PipelineConfig config = PipelineConfig::basic();
        config.fragShader = Shader::LoadShader("shader/gridfrag.spv");
        config.vertShader = Shader::LoadShader("shader/gridvert.spv");

        config.vertexInputInfo.vertexBindingDescriptionCount = 0;
        config.vertexInputInfo.pVertexBindingDescriptions = 0;
        config.vertexInputInfo.vertexAttributeDescriptionCount = 0;
        config.vertexInputInfo.pVertexAttributeDescriptions = 0;
        config.renderPass = renderPass;
        
        VkPushConstantRange range;
        range.offset = 0;
        range.size = sizeof(GridData);
        range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        pipeline.CreateLayout(&setLayout, 1, &range, 1);
        pipeline.Create(config);
	}
   




	void clean() 
    {
        pipeline.Clean();
        vkDestroyDescriptorSetLayout(Device::Instance().device, setLayout, NULL);
	}




};

