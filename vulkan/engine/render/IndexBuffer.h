#pragma once
#include "VBuffer.h"
class IndexBuffer:public VBuffer
{

public:	
	IndexBuffer(){}
	uint32_t indexCount =0;
	void allocBuffer(uint32_t size);
	void updateIndex(void* indexBuf, int size);
	// 通过 VBuffer 继承
	void alloc(uint32_t size) override;
};

