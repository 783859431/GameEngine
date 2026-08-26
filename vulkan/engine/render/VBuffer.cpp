#include "VBuffer.h"
#include "../logger/Logger.h"
void VBuffer::copy(void* data, uint32_t size, uint32_t offset)
{
	if (size + offset <= bf.size)
		memcpy((char*)bf.mapped + offset, data, size);
	else
		Logger::getInstance().log(PERROR,"memory out of range.");
}

void VBuffer::clean()
{
	if(bf._allocation)
		Allocator::freeBuffer(bf);

}
