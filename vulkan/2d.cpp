#include "vulkan/vulkan.h"
struct tri_vert
{

};

void DrawTriangle2D(tri_vert& verts) {
	 


	// 将所有要绘制的东西放到一个buffer中一起绘制，减少draw call的次数
	// 
	// Record commands to draw a triangle
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, /* pipeline */);
	vkCmdDraw(commandBuffer, 3, 1, 0, 0);
}