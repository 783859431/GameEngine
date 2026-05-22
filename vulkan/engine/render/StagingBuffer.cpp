#include "StagingBuffer.h"
#include <algorithm>
void StagingBuffer::allocBuffer(uint32_t size)
{
	m_allocBuf.size = size;
	Allocator::allocStagingBuffer(m_allocBuf);
}

void StagingBuffer::copyTempData(char* data,int size)
{
	int s = std::min(size, (int)m_allocBuf.size);
	memcpy(m_allocBuf.mapped, data, s);
}

void StagingBuffer::clean() {

	//Allocator::unMapBuffer(m_allocBuf);
	Allocator::freeBuffer(m_allocBuf);
}


void StagingBuffer::copyToImage(VkImage dst, int width, int height)
{

	CommandBuffer commadBuffer;
	commadBuffer.create();
	commadBuffer.begainOnce();
	VkBufferImageCopy region{};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;
	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;
	region.imageOffset = { 0, 0, 0 };
	region.imageExtent = {
		(uint32_t)width,
		(uint32_t)height,
		1
	};

	vkCmdCopyBufferToImage(commadBuffer.m_command, m_allocBuf._buffer, dst, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
	commadBuffer.endOnce();


}


void StagingBuffer::copyToBuffer(AllocatedBuffer& dst)
{
	VkBufferCopy region;
	region.dstOffset = 0;
	region.srcOffset = 0;
	region.size = dst.size;

	CommandBuffer commadBuffer;
	commadBuffer.create();
	commadBuffer.begainOnce();
	vkCmdCopyBuffer(commadBuffer.m_command, m_allocBuf._buffer, dst._buffer, 1, &region);
	commadBuffer.endOnce();

}
