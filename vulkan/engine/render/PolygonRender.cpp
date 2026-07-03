#include "PolygonRender.h"
#include <array>
#include "DescriptorSetManager.h"
void PolygonRender::createDescriptorSet()
{
    DescriptorSetLayoutBuilder builder;
    setLayout_cir = builder
        .AddBinding(0,
            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
            VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 1)
        .build();


    PoolManager::inst().allocSet(setLayout_cir,&set_cir);


    WriteSetHelper helper;
    VkDescriptorBufferInfo bf;
    bf.buffer = this->ubo.getBuffer();
    bf.offset = 0;
    bf.range =8;
    helper
        .AddWriteBuffer(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, set_cir, &bf)
        .Update();

}

void PolygonRender::init(VkDescriptorSetLayout lay, VkRenderPass pass)
{
 
	vtx.allocBuffer(maxVertCount*sizeof(Point));
    ubo.allocBuffer(maxCircleCount*Device::getInstance().uniformAlign);
    createDescriptorSet();
    createPipeline(pass,lay);
}

void PolygonRender::setLineWidth(float w)
{
    vkCmdSetLineWidth(cmd->m_command,w);
}



void PolygonRender::createPipeline(VkRenderPass pass, VkDescriptorSetLayout setLayout)
{
	PipelineConfig cf = PipelineConfig::basic();

    std::array<VkVertexInputBindingDescription, 1> binding{};
    binding[0].binding = 0;
    binding[0].stride = sizeof(Point);
    binding[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;


    std::array<VkVertexInputAttributeDescription, 1> attr{};
    attr[0].binding = 0;
    attr[0].location = 0;
    attr[0].format = VK_FORMAT_R32G32_SFLOAT;
    attr[0].offset = 0;

	cf.vertexInputInfo.vertexAttributeDescriptionCount = attr.size();
	cf.vertexInputInfo.vertexBindingDescriptionCount = binding.size();
	cf.vertexInputInfo.pVertexAttributeDescriptions = attr.data();
	cf.vertexInputInfo.pVertexBindingDescriptions = binding.data();
    
    VkPushConstantRange range;
    range.offset = 0;
    range.size = sizeof(glm::mat4);
    range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    cf.pushConstants.push_back(range);

    cf.renderPass = pass;
    cf.setLayouts.push_back(setLayout);
    cf.rasterizer.lineWidth = 2;
    cf.rasterizer.cullMode = VK_CULL_MODE_NONE;
    cf.rasterizer.polygonMode = VK_POLYGON_MODE_LINE;
    cf.inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;

    cf.fragShader = Shader::LoadShader("shader/lineFrag.spv");
    cf.vertShader = Shader::LoadShader("shader/lineVert.spv");

    pipeline.create(cf);

    
    cf.vertexInputInfo.vertexAttributeDescriptionCount = 0;
    cf.vertexInputInfo.vertexBindingDescriptionCount = 0;
    cf.vertexInputInfo.pVertexAttributeDescriptions =0;
    cf.vertexInputInfo.pVertexBindingDescriptions = 0;
    cf.rasterizer.lineWidth = 1;
    cf.rasterizer.cullMode = VK_CULL_MODE_NONE;
    cf.rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    cf.inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    cf.fragShader = Shader::LoadShader("shader/circleFrag.spv");
    cf.vertShader = Shader::LoadShader("shader/circleVert.spv");
    cf.setLayouts.push_back(this->setLayout_cir.layout);
    pipeline_cir.create(cf);
}

void PolygonRender::drawPolygon(Point* points, int count, Transform2d& trans)
{

    PolygonDrawData p;
    p.transform = trans;
    for (int i = 0; i < count; i++)
    {
        p.points.push_back(points[i]);
    }
    p.points.push_back(points[0]);
    polys.push_back(p);

}

void PolygonRender::drawCircle(float cx, float cy, float rad, float thickness , Transform2d& trans)
{
    CircleDrawData drawData;
    drawData.transform = trans;
    drawData.circle.c.x = cx;
    drawData.circle.c.y = cy;
    drawData.circle.rad = rad;
    drawData.circle.thickness = thickness;
    cirs.push_back(drawData);
}



void PolygonRender::flushPloy()
{
    int offset = 0;
    int index = 0;
    glm::mat4 transform(1.0f);
    setLineWidth(2);
    for (int i = 0; i < polys.size(); i++)
    {
        transform[0][0] = polys[i].transform.c;
        transform[0][1] = polys[i].transform.s;
        transform[1][0] = -polys[i].transform.s;
        transform[1][1] = polys[i].transform.c;
        transform[3][0] = polys[i].transform.x ;
        transform[3][1] = polys[i].transform.y;
        int vcount = polys[i].points.size();
        int size = vcount * sizeof(Point);
        vtx.updateVextex(polys[i].points.data(), size,offset);
        offset += size;
        cmd->bindPipeLine(this->pipeline.get());
        cmd->bindVertex(vtx.getBuffer());
        cmd->pushConst(
            pipeline.getLayout(), 
            VK_SHADER_STAGE_VERTEX_BIT, 
            &transform, 
            sizeof(transform));
        cmd->draw(vcount,1,index);
        index += vcount;
    }
    polys.clear();
}

void PolygonRender::flushCircle()
{

    glm::mat4 transform(1.0f);
    uint32_t doffset = 0;
    
    for (int i = 0; i < cirs.size(); i++)
    {
        float r = cirs[i].circle.rad;

        transform[0][0] = cirs[i].transform.c*r;
        transform[0][1] = cirs[i].transform.s*r;
        transform[1][0] = -cirs[i].transform.s*r;
        transform[1][1] = cirs[i].transform.c*r;
        transform[3][0] = cirs[i].transform.x;
        transform[3][1] = cirs[i].transform.y;
    
        ubo.updateData(&(cirs[i].circle.rad),8,doffset);
        cmd->bindPipeLine(this->pipeline_cir.get());
        cmd->bindSets(this->pipeline_cir.getLayout(), &set_cir, 1, 1, 1, &doffset);
        cmd->pushConst(
            pipeline_cir.getLayout(),
            VK_SHADER_STAGE_VERTEX_BIT,
            &transform,
            sizeof(transform));
        cmd->draw(6);
        doffset += 64;
    }

    cirs.clear();
}


