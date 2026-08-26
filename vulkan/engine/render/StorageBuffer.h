#pragma once
#include "VBuffer.h"
class StorageBuffer:public VBuffer
{
public:

	void allocBuffer(uint32_t size);
	void updateData(void* uniformData, int size, int offset = 0);
	VkDescriptorBufferInfo getDescriptorInfo();

	// 通过 VBuffer 继承
	void alloc(uint32_t size) override;

};

