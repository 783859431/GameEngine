#include "Material.h"


void Material::init()
{
    createDescriptorSets();
}

void Material::setPipeline(Pipeline* pipeline)
{
    this->pipline = pipeline;
}

void Material::setTexture(Texture* texture)
{
    this->baseTex = texture;
    if (baseTex)
    {
        updateDescriptorSets();
    }
}

void Material::bind(CommandBuffer& cmdBuffer, uint32_t currentFrame )
{
    cmdBuffer.bindPipeLine(this->pipline->get());
    cmdBuffer.bindSets(this->pipline->getLayout(),this->m_sets[currentFrame].data(), m_sets[currentFrame].size(),1);
}

void Material::updateDescriptorSets() 
{
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        WriteSetHelper helper;
        auto imgInfo = baseTex->getDescriptorImageInfo();
        helper
            // .AddWriteBuffer(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, m_sets[0], &bf)
            .AddWriteImage(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, m_sets[i][0], &imgInfo, 1)
            .Update();

    }

}

void Material::createDescriptorSets()
{
    m_setLayouts.resize(1);
    
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        m_sets[i].resize(1);
    DescriptorSetLayoutBuilder builder2;
    m_setLayouts[0] = builder2.AddBinding(
            0,
            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            VK_SHADER_STAGE_FRAGMENT_BIT,
            1)
        .build();
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {

        PoolManager::inst().allocSets(m_setLayouts,m_sets[i]);
    }


}

void Material::clean()
{
    for (int i = 0; i < m_setLayouts.size(); i++)
    {
      if(m_setLayouts[i].layout!=0)
          vkDestroyDescriptorSetLayout(Device::getInstance().device, m_setLayouts[i].layout, 0);
    }

}
