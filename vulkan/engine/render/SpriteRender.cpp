#include "SpriteRender.h"
#include "DescriptorSetManager.h"
#include "../Global.h"
extern VkDescriptorSet g_set0[];
extern VkDescriptorSetLayout g_layout;

void SpriteRender::createDescriptorSet()
{
    DescriptorSetLayoutBuilder builder2;
    setLayout = builder2
        .AddBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 1)
        .build();
    setLayout2 = builder2
        .AddBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 1)
        .build();
    std::vector<DSetLayout*> lays;

    DoTimes([&](int i) {
        lays.push_back(&setLayout);
        });

    PoolManager::inst().allocSets(lays.data(), (int)lays.size(),sets);

    DoTimes([&](int i){
        sbuf[i].allocBuffer(sizeof(InstanceData) * 100);
        auto bufferInfo = sbuf[i].getDescriptorInfo();
        WriteSetHelper helper;
        helper.AddWriteBuffer(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, sets[i], &bufferInfo).Update();
    });

  

}
SpriteRender::~SpriteRender()
{
    this->clean();
}
void SpriteRender::init(VkRenderPass pass)
{
    createDescriptorSet();
    createPipeline(pass);

}


void SpriteRender::createPipeline(VkRenderPass pass)
{
    PipelineConfig cf = PipelineConfig::basic();

    cf.fragShader = Shader::LoadShader("shader/spriteFrag.spv");
    cf.vertShader = Shader::LoadShader("shader/spriteVert.spv");
    cf.renderPass = pass;
 
    cf.setLayouts.push_back(g_layout);
    cf.setLayouts.push_back(setLayout.layout);
    cf.setLayouts.push_back(setLayout2.layout);

    spritePipeline.create(cf);


}


void SpriteRender::drawSprite(Sprite* sp)
{
    InstanceData data;
    data.transform = {sp->transform.x,sp->transform.y,sp->transform.c,sp->transform.s};
    data.rect = sp->rawPos;
    data.sheetWH = { sp->texture->width,sp->texture->height,PixelToUnit(sp->w), PixelToUnit(sp->h) };
    diffTex[sp->texture].push_back(data);


}


void SpriteRender::flush(CommandBuffer& cmd, int frame)
{

    cmd.bindPipeLine(this->spritePipeline.get());
    cmd.bindSets(this->spritePipeline.getLayout(), &g_set0[frame], 1, 0);
    cmd.bindSets(this->spritePipeline.getLayout(), &sets[frame], 1, 1);
    int offset = 0;
    for (auto& [k, v] : diffTex)
    {
      updateTexture(cmd, k);
      sbuf[frame].updateData(v.data(), v.size() * sizeof(InstanceData), offset * sizeof(InstanceData));
      cmd.draw(6, v.size(), 0,offset);
      offset += v.size();
    }
   
    diffTex.clear();

    
}

void SpriteRender::clean()
{
    auto dev = Device::getInstance().device;
    vkDestroyDescriptorSetLayout(dev, setLayout.layout,NULL);
    vkDestroyDescriptorSetLayout(dev, setLayout2.layout, NULL);
}


void SpriteRender::updateTexture(CommandBuffer& cmd, Texture* tex)
{
    auto set = tex->getSet();
    cmd.bindSets(this->spritePipeline.getLayout(), &set, 1, 2);
}
