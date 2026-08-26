#pragma once
#include "../logger/Logger.h"
#include "VBuffer.h"
class VertexBuffer:public VBuffer
{
	
public:	
	int vertCount = 0;
	void allocBuffer(uint32_t size);
	void allocBuffer(uint32_t count, uint32_t vertSize);
	void updateVextex(void* vertexsBuf,int size, int offset = 0);
	~VertexBuffer() {
		
	}

	void alloc(uint32_t size) override;

};

