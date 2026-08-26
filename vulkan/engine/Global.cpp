#include "Global.h"
#include "vulkan/vulkan.h"
#include "./render/Device.h"
 float PixelToUnit(float pixels)
{
	return pixels / PIXEL_PER_UNIT;
}

 float UnitToPixel(float unit)
{
	return unit * PIXEL_PER_UNIT;
}


//几乎所有pipeline都需要这个set
VkDescriptorSet g_set0[MAX_FRAMES_IN_FLIGHT];
VkDescriptorSetLayout g_layout;
VkRenderPass g_renderPass;
