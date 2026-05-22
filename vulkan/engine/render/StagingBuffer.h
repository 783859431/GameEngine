#pragma once
#include "vulkan/vulkan.h"
#include "vk_mem_alloc.h"
#include "Allocator.h"
#include <memory>
#include "CommandBuffer.h"
class StagingBuffer
{

	AllocatedBuffer m_allocBuf;
public :
	void allocBuffer(uint32_t size);

	void copyTempData(char* data,int size);

	void clean();

	void copyToImage(VkImage dst, int width, int height);


	void copyToBuffer(AllocatedBuffer& dst);

	~StagingBuffer()
	{
		if(m_allocBuf._allocation)
			clean();
	}
	VkBuffer getBuffer(){
		return m_allocBuf._buffer;
	}

};

