#include "StorageBuffer.h"

void StorageBuffer::allocBuffer(uint32_t size)
{
	m_allocBuf.size = size;
	Allocator::allocStorageBuffer(m_allocBuf);

}

void StorageBuffer::updateData(void* uniformData, int size, int offset)
{
	int s = std::min(size + offset, (int)m_allocBuf.size);
	memcpy((char*)m_allocBuf.mapped + offset, uniformData, s);

}

VkDescriptorBufferInfo StorageBuffer::getDescriptorInfo()
{
	VkDescriptorBufferInfo info;
	info.buffer = this->getBuffer();
	info.offset = 0;
	info.range = m_allocBuf.size;
	return info;
}

VkBuffer StorageBuffer::getBuffer()
{
	return m_allocBuf._buffer;

}

void StorageBuffer::clean()
{
	Allocator::freeBuffer(m_allocBuf);
}
