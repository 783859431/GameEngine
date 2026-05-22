#pragma once
#include <vulkan/vulkan.hpp>
class Sampler
{
	VkSampler sampler =0;
public :
	void createDefault();
	void create();
	void clean();
	~Sampler() {
		if (sampler)
			clean();
	}

};

