#pragma once
#include "vulkan/vulkan.h"
#include "vk_mem_alloc.h"
#include "Allocator.h"
#include <memory>
class IndexBuffer
{

	AllocatedBuffer allocBuf;


public:	
	uint32_t indexCount;
	void AllocBuffer(uint32_t size);
	void CopyVextex(char* indexBuf);
	VkBuffer GetBuffer();

	void Clean();
};

