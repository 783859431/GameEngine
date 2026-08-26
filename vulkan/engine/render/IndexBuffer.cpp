#include "IndexBuffer.h"

void IndexBuffer::allocBuffer(uint32_t size)
{
	alloc(size);
}

void IndexBuffer::updateIndex(void* indexBuf, int size)
{
	copy(indexBuf, size);
}
void IndexBuffer::alloc(uint32_t size)
{
	bf.size = size;
	indexCount = size / sizeof(int);
	Allocator::allocIndexBuffer(bf);

}
