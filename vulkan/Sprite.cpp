#include "Sprite.h"
#include "Input.h"
#include "glWindow.h"
#include "Global.h"
void SpriteRender::init(VkRenderPass renderPass) {

    spts.clear();
    //
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        vb[i].AllocBuffer(sizeof(SpriteVertex) * 6);
        ub[i].AllocBuffer(sizeof(MVP));
    }


    DescriptorSetLayoutBuilder builder1;
    setLayout = builder1
        .SetDevice(Device::Instance().device)
        .AddBinding(0,VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT,1)
        .AddBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT,200)
        .build();
    VkDescriptorSetLayout layouts[2];
    layouts[0] = layouts[1] = setLayout;
    DescriptorSetPool::Instance().AllocSets(2, &layouts[0], &set[0]);


    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        WriteSetHelper helper;
        VkDescriptorBufferInfo bf;
        bf.buffer = ub[i].GetBuffer();
        bf.offset = 0;
        bf.range = sizeof(MVP);
        helper
            .AddWriteBuffer(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, set[i], &bf)
            .Update();
    }


    //pipeline
    PipelineConfig config = PipelineConfig::basic();
    config.fragShader = Shader::LoadShader("shader/imgF.spv");
    config.vertShader = Shader::LoadShader("shader/imgV.spv");

    auto binding = SpriteVertex::getBindingDescription();
    auto desc = SpriteVertex::getAttributeDescriptions();

    config.vertexInputInfo.vertexBindingDescriptionCount = 1;
    config.vertexInputInfo.pVertexBindingDescriptions = &binding;
    config.vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(desc.size());
    config.vertexInputInfo.pVertexAttributeDescriptions = desc.data();
    config.renderPass = renderPass;

    VkPushConstantRange range;
    range.offset = 0;
    range.size = sizeof(PushData);
    range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    config.setLayouts.push_back(setLayout);
    config.pushConstants.push_back(range);
    pipeline.Create(config);
}

void SpriteRender::Draw(CommandBuffer& cmd,Sprite& sp, int currentFrame) {


    //四个顶点
    glm::vec2 p1 = -sp.trans2d.pivot;
    glm::vec2 p2 =  glm::vec2(sp.width-sp.trans2d.pivot.x, sp.height-sp.trans2d.pivot.y);
    glm::vec2 p3 = { p1.x,p2.y };
    glm::vec2 p4 = { p2.x,p1.y };
    //两个三角形
    SpriteVertex v[6] =
    {
       { p1, {0,0} }, // 左上
       { p4, {1,0} }, // 右上
       { p3, {0,1} }, // 左下

       { p4, {1,0} }, // 右上
       { p2, {1,1} }, // 右下
       { p3, {0,1} }  // 左下
    };

    PushData data = {glWindow::Instance().GetFrameSize(),sp.index};
    MVP mats;
    mats.model = sp.trans2d.GetModel();
    mats.view = g_Cam2d.GetView();
    mats.proj = g_Cam2d.GetProject();

    ub[currentFrame].UpdateData(&mats);//更新矩阵
    cmd.PushConst(pipeline.GetLayout(), VK_SHADER_STAGE_VERTEX_BIT, &data, sizeof(PushData));
    vb[currentFrame].UpdateVextex(&v[0]);//更新顶点
    
    cmd.BindPipeLine(pipeline.get());
    cmd.BindVertex(&vb[currentFrame].GetBuffer());
    cmd.BindSets(pipeline.GetLayout(), &set[currentFrame], 1);
    cmd.Draw();

}

void SpriteRender::AddSprite(Sprite* spt)
{
    WriteSetHelper helper;
    VkDescriptorImageInfo img_info = spt->getDescriptorInfo();
    spt->index = spts.size();
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        helper
        .AddWriteImage(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, set[i], &img_info, 1, spt->index)
        .Update();//更新图片数据
    }
    spts.push_back(spt);
}

void SpriteRender::DrawSprites(CommandBuffer& cmd,int currentFrame)
{
    for (int i = 0; i < spts.size(); i++)
        Draw(cmd,*spts[i],currentFrame);
}

void SpriteRender::Clean()
{
    pipeline.Clean();
    vkDestroyDescriptorSetLayout(Device::Instance().device, setLayout,NULL);
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        vb[i].Clean();
        ub[i].Clean();
    }

}

void Sprite::Clean()
{
    img.Clean();
}
