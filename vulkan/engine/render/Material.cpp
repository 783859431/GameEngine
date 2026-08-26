#include "Material.h"


void Material::init()
{
  //  createDescriptorSets();
}

void Material::setPipeline(Pipeline* pipeline)
{
    this->pipline = pipeline;
}



void Material::bind(CommandBuffer& cmdBuffer, uint32_t currentFrame )
{
    cmdBuffer.bindPipeLine(this->pipline->get());
   // cmdBuffer.bindSets(this->pipline->getLayout(),,,1);
}




void Material::clean()
{

   //  vkDestroyDescriptorSetLayout(Device::getInstance().device, m_setLayout.layout, 0);


}
