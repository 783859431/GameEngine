#pragma once
#include <vulkan/vulkan.hpp>

enum  SamplerType
{
	LinearRepeat,
	LinearClamp,
	NearestRepeat,
	// ShadowCompare,
	// PointClamp

};


class SamplerManager
{
public :
	static void createDefaultSamplers();
	static void clean();
	static VkSampler getSamper(SamplerType tp);
};