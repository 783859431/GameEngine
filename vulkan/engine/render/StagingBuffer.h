#pragma once
#include "VBuffer.h"
#include "CommandBuffer.h"
class StagingBuffer:public VBuffer
{

public :
	void allocBuffer(uint32_t size);

	void copyTempData(char* data,int size);

	void copyToImage(VkImage dst, int width, int height);

	void copyToBuffer(AllocatedBuffer& dst);

	// 通过 VBuffer 继承
	void alloc(uint32_t size) override;

};

