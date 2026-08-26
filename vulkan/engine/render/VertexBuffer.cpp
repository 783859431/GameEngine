#include "VertexBuffer.h"
void VertexBuffer::allocBuffer(uint32_t size)
{
	alloc(size);
}

void VertexBuffer::allocBuffer(uint32_t count, uint32_t vertSize)
{
	alloc(count * vertSize);
	vertCount = count;
}

void VertexBuffer::updateVextex(void* vertexsBuf,int size ,int offset)
{
	copy(vertexsBuf,size,offset);
}

void VertexBuffer::alloc(uint32_t size)
{
	bf.size = size;
	Allocator::allocVertexBuffer(bf);
}
