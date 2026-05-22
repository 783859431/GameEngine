#include "VertexBuffer.h"
void VertexBuffer::allocBuffer(uint32_t size)
{
	m_allocBuf.size = size;
	Allocator::allocVertexBuffer(m_allocBuf);
}
void VertexBuffer::allocBuffer(uint32_t count, uint32_t vertSize)
{
	m_allocBuf.size = count * vertSize;

	Allocator::allocVertexBuffer(m_allocBuf);
	vertCount = count;
}

void VertexBuffer::updateVextex(void* vertexsBuf,int size)
{
	int s = std::min(size, (int)m_allocBuf.size);
	memcpy(m_allocBuf.mapped, vertexsBuf, s);
}

void VertexBuffer::clean() {

	//Allocator::unMapBuffer(m_allocBuf);
	Allocator::freeBuffer(m_allocBuf);
}