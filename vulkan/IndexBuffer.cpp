#include "IndexBuffer.h"

void IndexBuffer::AllocBuffer(uint32_t size)
{
	allocBuf.size = size;
	indexCount = size / sizeof(int);
	Allocator::Instance().AllocIndexBuffer(allocBuf);
	Allocator::Instance().MapBuffer(allocBuf); // GPU can access this memory 
}

void IndexBuffer::CopyVextex(char* indexBuf)
{
	memcpy(allocBuf.mapped, indexBuf, allocBuf.size);
}
VkBuffer IndexBuffer::GetBuffer()
{
	return allocBuf._buffer;
}

void IndexBuffer::Clean() {

	Allocator::Instance().UnMapBuffer(allocBuf);
	Allocator::Instance().FreeBuffer(allocBuf);
}