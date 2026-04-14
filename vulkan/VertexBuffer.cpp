#include "VertexBuffer.h"
void VertexBuffer::AllocBuffer(uint32_t size)
{
	allocBuf.size = size;
	Allocator::Instance().AllocVertexBuffer(allocBuf);
	Allocator::Instance().MapBuffer(allocBuf); // GPU can access this memory 
}

void VertexBuffer::UpdateVextex(void* vertexsBuf)
{
	memcpy(allocBuf.mapped, vertexsBuf, allocBuf.size);
}
VkBuffer& VertexBuffer::GetBuffer()
{
	return allocBuf._buffer;
}
void VertexBuffer::Clean() {

	Allocator::Instance().UnMapBuffer(allocBuf);
	Allocator::Instance().FreeBuffer(allocBuf);
}