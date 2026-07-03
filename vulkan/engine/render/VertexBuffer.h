#pragma once
#include "vulkan/vulkan.h"
#include "vk_mem_alloc.h"
#include "Allocator.h"
#include <memory>
#include "../logger/Logger.h"
class VertexBuffer
{
	AllocatedBuffer m_allocBuf;
	
public:
	void allocBuffer(uint32_t size);
	void allocBuffer(uint32_t count, uint32_t vertSize);
	int vertCount = 0;
	void updateVextex(void* vertexsBuf,int size, int offset = 0);
	void clean();
	~VertexBuffer() {
		if(m_allocBuf._allocation)
			clean();
	}
	VkBuffer getBuffer() {
		return m_allocBuf._buffer;
	}

};

