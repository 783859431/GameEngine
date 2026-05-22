#pragma once
#include "vulkan/vulkan.h"
#include "vk_mem_alloc.h"
#include "Allocator.h"
#include <memory>
class IndexBuffer
{

	AllocatedBuffer allocBuf;
public:	
	uint32_t indexCount =0;
	void allocBuffer(uint32_t size);
	void updateIndex(void* indexBuf, int size);
	VkBuffer getBuffer();
	void clean();
	~IndexBuffer()
	{
		if (allocBuf._allocation)
			clean();
	}
};

