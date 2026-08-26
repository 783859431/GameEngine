#include "PolygonRender.h"
#include <array>
#include "DescriptorSetManager.h"


extern VkDescriptorSet g_set0[];
extern VkDescriptorSetLayout g_layout;

void PolygonRender::init( VkRenderPass pass)
{
 
    DoTimes([&](int i) {
        vtx_poly[i].allocBuffer(maxPolyCount * sizeof(PolygenInstance));
        vtx_circle[i].allocBuffer(maxCircleCount * sizeof(CircleInstance));
        vtx_line[i].allocBuffer(maxLineCount * sizeof(Line));
        });
	
    createPipeline(pass);
}

void PolygonRender::setLineWidth(CommandBuffer& cmd, float w)
{
    vkCmdSetLineWidth(cmd.m_command,w);
}



void PolygonRender::createPipeline(VkRenderPass pass)
{
	PipelineConfig cf = PipelineConfig::basic();

    auto desp = PolygenInstance::getDescription();
    auto binding = PolygenInstance::getBinding();

	cf.vertexInputInfo.vertexAttributeDescriptionCount = desp.size();
	cf.vertexInputInfo.vertexBindingDescriptionCount = binding.size();
	cf.vertexInputInfo.pVertexAttributeDescriptions = desp.data();
	cf.vertexInputInfo.pVertexBindingDescriptions = binding.data();
    
    cf.renderPass = pass;
    cf.setLayouts.push_back(g_layout);

    cf.fragShader = Shader::LoadShader("shader/polyFrag.spv");
    cf.vertShader = Shader::LoadShader("shader/polyVert.spv");

    polyPipeline.create(cf);

    PipelineConfig cf2 = PipelineConfig::basic();
    auto desp2 = CircleInstance::getDescription();
    auto binding2 = CircleInstance::getBinding();

    cf2.vertexInputInfo.vertexAttributeDescriptionCount = desp2.size();
    cf2.vertexInputInfo.vertexBindingDescriptionCount = binding2.size();
    cf2.vertexInputInfo.pVertexAttributeDescriptions = desp2.data();
    cf2.vertexInputInfo.pVertexBindingDescriptions = binding2.data();

    cf2.renderPass = pass;
    cf2.setLayouts.push_back(g_layout);

    cf2.fragShader = Shader::LoadShader("shader/circleFrag.spv");
    cf2.vertShader = Shader::LoadShader("shader/circleVert.spv");
    pipeline_cir.create(cf2);
    

    PipelineConfig cf3 = PipelineConfig::basic();
    auto desp3 = Line::getDescription();
    auto binding3 = Line::getBinding();

    cf3.vertexInputInfo.vertexAttributeDescriptionCount = desp3.size();
    cf3.vertexInputInfo.vertexBindingDescriptionCount = binding3.size();
    cf3.vertexInputInfo.pVertexAttributeDescriptions = desp3.data();
    cf3.vertexInputInfo.pVertexBindingDescriptions = binding3.data();

    cf3.renderPass = pass;
    cf3.setLayouts.push_back(g_layout);

    cf3.fragShader = Shader::LoadShader("shader/lineFrag.spv");
    cf3.vertShader = Shader::LoadShader("shader/lineVert.spv");
    pipeline_line.create(cf3);

}

void PolygonRender::drawPolygon(const Transform2d& transform, const Point* points, int count, float radius, const glm::vec4& color)
{

    PolygenInstance p;
    p.color = color;
    p.transform = {transform.x,transform.y,transform.c,transform.s};
  
    int n = count < 8 ? count : 8;
    glm::vec2* pv2 = (glm::vec2*) & p.point12;

    for (int i = 0; i < n; ++i)
    {
        pv2[i] = {points[i].x,points[i].y};
    }
    p.pointsCount = n;
    p.rad = radius;

    polys.push_back(p);

}

void PolygonRender::drawCircle(const Transform2d& trans, float rad, const glm::vec4& color)
{

    CircleInstance circle;
    circle.transform = { trans.x,trans.y,trans.c,trans.s };
    circle.radius = rad;
    circle.color = color;
    cirs.push_back(circle);
}

void PolygonRender::drawLine(const Point& pt1, const Point& pt2, const glm::vec4& color)
{
    Line line;
    line.p1 = pt1;
    line.p2 = pt2;
    line.color = color;
    lines.push_back(line);
}

void PolygonRender::flushPloys(CommandBuffer& cmd,int frame)
{
    if (polys.size() == 0) return;
    cmd.bindPipeLine(this->polyPipeline.get());
    cmd.bindSets(this->polyPipeline.getLayout(), g_set0, 1);
    cmd.bindVertex(vtx_poly[frame].getBuffer());
    vtx_poly[frame].updateVextex(polys.data(), polys.size() * sizeof(PolygenInstance));
    cmd.draw(6, polys.size());
    polys.clear();
}

void PolygonRender::flushCircles(CommandBuffer& cmd,int frame)
{
    if (cirs.size() == 0) return;
    cmd.bindPipeLine(this->pipeline_cir.get());
    cmd.bindSets(this->pipeline_cir.getLayout(), g_set0, 1);
    cmd.bindVertex(vtx_circle[frame].getBuffer());
    vtx_circle[frame].updateVextex(cirs.data(), cirs.size() * sizeof(CircleInstance));
    cmd.draw(6, cirs.size());
    cirs.clear();
}

void PolygonRender::flushLines(CommandBuffer& cmd, int frame)
{
    if (lines.size() == 0) return;
    cmd.bindPipeLine(this->pipeline_line.get());
    cmd.bindSets(this->pipeline_line.getLayout(), g_set0, 1);
    cmd.bindVertex(vtx_line[frame].getBuffer());
    vtx_line[frame].updateVextex(lines.data(), lines.size() * sizeof(Line));
    cmd.draw(6, lines.size());
    lines.clear();

}

void PolygonRender::clean()
{
   
}


