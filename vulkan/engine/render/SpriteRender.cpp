#include "SpriteRender.h"
#include "DescriptorSetManager.h"
#include "../Global.h"

void SpriteRender::createDescriptorSet()
{
    DescriptorSetLayoutBuilder builder2;
    setLayout = builder2
        .AddBinding(0,VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,VK_SHADER_STAGE_VERTEX_BIT,1)
        .build();
    PoolManager::inst().allocSet(setLayout, &sets[1]);
    sbuf.allocBuffer(sizeof(InstanceData)*1024);
    auto bufferInfo =  sbuf.getDescriptorInfo();
    WriteSetHelper helper;
    helper.AddWriteBuffer(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,sets[1], &bufferInfo).Update();
  

}
void SpriteRender::init(VkRenderPass pass, VkDescriptorSetLayout setLayout0)
{
    createDescriptorSet();
    createPipeline(pass,setLayout0);
  

}


void SpriteRender::createPipeline(VkRenderPass pass, VkDescriptorSetLayout setLayout0)
{
    PipelineConfig cf = PipelineConfig::basic();

    cf.fragShader = Shader::LoadShader("shader/spriteFrag.spv");
    cf.vertShader = Shader::LoadShader("shader/spriteVert.spv");
    cf.renderPass = pass;
    DescriptorSetLayoutBuilder builder2;
    auto setLayout2 = builder2
        .AddBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 1)
        .build();
    cf.setLayouts.push_back(setLayout0);
    cf.setLayouts.push_back(setLayout.layout);
    cf.setLayouts.push_back(setLayout2.layout);

    spritePipeline.create(cf);


}


void SpriteRender::drawSprite(Sprite* sp)
{
    InstanceData data;
    glm::mat4 transform(1.0f);
    transform[0][0] = sp->transform.c * PixelToUnit(sp->w);
    transform[0][1] = sp->transform.s * PixelToUnit(sp->w);
    transform[1][0] = -sp->transform.s * PixelToUnit(sp->h);
    transform[1][1] = sp->transform.c * PixelToUnit(sp->h);
    transform[3][0] = sp->transform.x;
    transform[3][1] = sp->transform.y;
    data.model = transform;
    data.rect = sp->rawPos;
    data.sheetWH = { sp->texture->width,sp->texture->height,0,0 };
    diffTex[sp->texture].push_back(data);


}


void SpriteRender::flush(CommandBuffer& cmd)
{

    cmd.bindPipeLine(this->spritePipeline.get());
    cmd.bindSets(this->spritePipeline.getLayout(), &sets[0], 2, 0);
    int offset = 0;
    for (auto& [k, v] : diffTex)
    {
      updateTexture(cmd, k);
      sbuf.updateData(v.data(),v.size()*sizeof(InstanceData),offset*sizeof(InstanceData));
      cmd.draw(6, v.size(), 0,offset);
      offset += v.size();
    }
   
    diffTex.clear();

    
}


void SpriteRender::updateTexture(CommandBuffer& cmd, Texture* tex)
{
    auto set = tex->getSet();
    cmd.bindSets(this->spritePipeline.getLayout(), &set, 1, 2);
}
