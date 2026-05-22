#pragma once
#include "vulkan/vulkan.h"
#include "vk_mem_alloc.h"
#include "Allocator.h"
#include <memory>
class UniformBuffer
{
	AllocatedBuffer m_allocBuf;
public:
	
	void allocBuffer(uint32_t size);
	void updateData(void* uniformData, int size);
	VkDescriptorBufferInfo getDescriptorInfo();
	VkBuffer getBuffer();
	void clean();
	~UniformBuffer() {
		if(m_allocBuf._allocation)
			clean();
	}
};

