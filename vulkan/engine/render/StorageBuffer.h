#pragma once
#include "vulkan/vulkan.h"
#include "vk_mem_alloc.h"
#include "Allocator.h"
#include <memory>
class StorageBuffer
{

	AllocatedBuffer m_allocBuf;
public:

	void allocBuffer(uint32_t size);
	void updateData(void* uniformData, int size, int offset = 0);
	VkDescriptorBufferInfo getDescriptorInfo();
	VkBuffer getBuffer();
	void clean();
	~StorageBuffer() {
		if (m_allocBuf._allocation)
			clean();
	}
};

