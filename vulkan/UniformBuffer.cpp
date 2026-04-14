#include "UniformBuffer.h"
void UniformBuffer::AllocBuffer(uint32_t size)
{
	allocBuf.size = size;
	Allocator::Instance().AllocUniformBuffer(allocBuf);
	Allocator::Instance().MapBuffer(allocBuf); // GPU can access this memory 
}

void UniformBuffer::UpdateData(void* uniformData)
{
	memcpy(allocBuf.mapped, uniformData, allocBuf.size);
}

VkBuffer UniformBuffer::GetBuffer()
{
	return allocBuf._buffer;
}

void UniformBuffer::Clean()
{
	Allocator::Instance().UnMapBuffer(allocBuf);
	Allocator::Instance().FreeBuffer(allocBuf);
}
