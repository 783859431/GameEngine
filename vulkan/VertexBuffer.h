#pragma once
#include "vulkan/vulkan.h"
#include "vk_mem_alloc.h"
#include "Allocator.h"
#include <memory>
class VertexBuffer
{
	AllocatedBuffer allocBuf;
	
public:
	void AllocBuffer(uint32_t size);
	
	void UpdateVextex(void* vertexsBuf);
	VkBuffer& GetBuffer();
	void Clean();


};

