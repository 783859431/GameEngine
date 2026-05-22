#include "VObject.h"

ConstData c1;
void VObject::draw(CommandBuffer& cmd,int currentFrame)
{
	c1.m = transform.getModelMatrix();
	cmd.pushConst(material->pipline->getLayout(), VK_SHADER_STAGE_VERTEX_BIT, &c1, sizeof(ConstData));
	material->bind(cmd, currentFrame);
	mesh.drawIndexed(cmd);
}
