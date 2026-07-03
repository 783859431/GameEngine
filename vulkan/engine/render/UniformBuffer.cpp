#include "UniformBuffer.h"
#include <algorithm>
void UniformBuffer::allocBuffer(uint32_t size)
{
	m_allocBuf.size = size;
	Allocator::allocUniformBuffer(m_allocBuf); 
}

void UniformBuffer::updateData(void* uniformData,int size,int offset)
{
	int s = std::min(size + offset, (int)m_allocBuf.size);
	memcpy((char*)m_allocBuf.mapped + offset, uniformData, s);
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
