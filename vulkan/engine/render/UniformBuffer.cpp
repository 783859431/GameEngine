#include "UniformBuffer.h"
#include <algorithm>
void UniformBuffer::allocBuffer(uint32_t size)
{
	m_allocBuf.size = size;
	Allocator::allocUniformBuffer(m_allocBuf); 
}

void UniformBuffer::updateData(void* uniformData,int size)
{
	int s = std::min(size, (int)m_allocBuf.size);
	memcpy(m_allocBuf.mapped, uniformData, s);
}

VkDescriptorBufferInfo UniformBuffer::getDescriptorInfo()
{
	VkDescriptorBufferInfo info;
	info.buffer = this->getBuffer();
	info.offset = 0;
	info.range = m_allocBuf.size;
	return info;

}

VkBuffer UniformBuffer::getBuffer()
{
	return m_allocBuf._buffer;
}

void UniformBuffer::clean()
{
	//Allocator::unMapBuffer(m_allocBuf);
	Allocator::freeBuffer(m_allocBuf);
}
