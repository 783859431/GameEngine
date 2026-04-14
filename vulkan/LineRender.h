#pragma once
#include "vulkan/vulkan.h"
#include "glm/glm.hpp"
#include <vector>
#include <array>
#include "Utils.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "UniformBuffer.h"
#include "CommandBuffer.h"
#include "Pipeline.h"
#include "DescriptorSetManager.h"
struct LineVertex {
    glm::vec2 pos;
    int side;
    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(LineVertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(LineVertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(LineVertex, side);

        return attributeDescriptions;
    }
};

struct LineData
{
    alignas(4)  float linewidth;
    alignas(16) glm::vec4 color;
    alignas(8)  glm::vec2 viewport;
    alignas(8)  glm::vec2 dir; // padding µΩ 48 bytes
};

class LineRender
{
	Pipeline pipeline;
    IndexBuffer idx;
    VertexBuffer vtx;
    VkDescriptorSetLayout setLayout;

public :
    void init(VkRenderPass renderPass) {

        idx.AllocBuffer(6 * sizeof(int));
        vtx.AllocBuffer(4 * sizeof(LineVertex));

        DescriptorSetLayoutBuilder builder;
        setLayout = builder
            .SetDevice(Device::Instance().device)
            .build();


        PipelineConfig config = PipelineConfig::basic();
        config.fragShader = Shader::LoadShader("shader/linefrag.spv");
        config.vertShader = Shader::LoadShader("shader/linevert.spv");


        auto linebinding = LineVertex::getBindingDescription();
        auto lineDesc = LineVertex::getAttributeDescriptions();

        config.vertexInputInfo.vertexBindingDescriptionCount = 1;
        config.vertexInputInfo.pVertexBindingDescriptions = &linebinding;
        config.vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(lineDesc.size());
        config.vertexInputInfo.pVertexAttributeDescriptions = lineDesc.data();
        config.renderPass = renderPass;

        VkPushConstantRange range;
        range.offset = 0;
        range.size = sizeof(LineData);
        range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        pipeline.CreateLayout(&setLayout,1,&range,1);
        pipeline.Create(config);




    }
    void Clean() {

     
        idx.Clean();
        vtx.Clean();
        pipeline.Clean();
        vkDestroyDescriptorSetLayout(Device::Instance().device,setLayout,NULL);


    }

    void drawLine2d(CommandBuffer& cmd, const glm::vec2& point1, const glm::vec2& point2, float width, const glm::vec4& color) {


        glm::vec2 p1 = Utils::Screen2NDC(point1);
        glm::vec2 p2 = Utils::Screen2NDC(point2);//Utils::Screen2NDC(point2);        
        glm::vec2 dir = p2 - p1;

        std::vector<LineVertex> v = 
        {
            {p1,1},
            {p1,-1},
            {p2,1},
            {p2,-1}
        };
        std::vector<int> index = {
            0,1,2,2,1,3
        };

        vtx.CopyVextex((char*)v.data());
        idx.CopyVextex((char*)index.data());
        cmd.BindVertex(&vtx.GetBuffer());
        cmd.BindIndex(idx.GetBuffer());
        cmd.BindPipeLine(pipeline.get());

        LineData linedata{ width,color,glm::vec2(glWindow::Instance().width,glWindow::Instance().height),dir};
        cmd.PushConst(pipeline.GetLayout(), VK_SHADER_STAGE_VERTEX_BIT, (char*) & linedata, sizeof(LineData));
        cmd.DrawIndexed(idx.indexCount);

    }
    void DrawRect(CommandBuffer& cmd, const glm::vec4& rect, const glm::vec4& color) {
        auto p1 = glm::vec2(rect.x,rect.y);//◊Û…œΩ«
        auto p2 = glm::vec2(rect.x,rect.y + rect.w);//◊Ûœ¬Ω«
        auto p3 = glm::vec2(rect.x + rect.z, rect.y);//”“…œΩ«
        auto p4 = glm::vec2(rect.x + rect.z,rect.y + rect.w);//”“œ¬Ω«
        p1 = Utils::Screen2NDC(p1);
        p2 = Utils::Screen2NDC(p2);
        p3 = Utils::Screen2NDC(p3);
        p4 = Utils::Screen2NDC(p4);
        glm::vec2 v[4] = {
            p1,p2,p3,p4
        };
        int index[6] = {
            0,1,2,2,1,3
        };
      


    }

    struct Circle {

        glm::vec2 center;
        float r;
        float thickness;

    };

    void drawCircle(VkCommandBuffer cmd, const glm::vec2& center,  float r , float thickness = 2) {

        glm::vec2 c = Utils::Screen2NDC(center);
        /*
        
        
        */

    }

};

