#include "IndexBuffer.h"

void IndexBuffer::allocBuffer(uint32_t size)
{
	allocBuf.size = size;
	indexCount = size / sizeof(int);
	Allocator::allocIndexBuffer(allocBuf);
}

void IndexBuffer::updateIndex(void* indexBuf, int size)
{
	memcpy(allocBuf.mapped, indexBuf, size);
}
VkBuffer IndexBuffer::getBuffer()
{
	return allocBuf._buffer;
}

void IndexBuffer::clean() {

//	Allocator::unMapBuffer(allocBuf);
	Allocator::freeBuffer(allocBuf);
}