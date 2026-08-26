#include "StorageBuffer.h"

void StorageBuffer::allocBuffer(uint32_t size)
{
	alloc(size);

}

void StorageBuffer::updateData(void* uniformData, int size, int offset)
{
	copy(uniformData, size, offset);

}

VkDescriptorBufferInfo StorageBuffer::getDescriptorInfo()
{
	VkDescriptorBufferInfo info;
	info.buffer = this->getBuffer();
	info.offset = 0;
	info.range = bf.size;
	return info;
}

void StorageBuffer::alloc(uint32_t size)
{
	bf.size = size;
	Allocator::allocStorageBuffer(bf);

}
