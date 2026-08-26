#include "StagingBuffer.h"
#include <algorithm>
void StagingBuffer::allocBuffer(uint32_t size)
{
	alloc(size);
}

void StagingBuffer::copyTempData(char* data,int size)
{
	copy(data,size);
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

	vkCmdCopyBufferToImage(commadBuffer.m_command, bf._buffer, dst, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
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
	vkCmdCopyBuffer(commadBuffer.m_command, bf._buffer, dst._buffer, 1, &region);
	commadBuffer.endOnce();

}

void StagingBuffer::alloc(uint32_t size)
{
	bf.size = size;
	Allocator::allocStagingBuffer(bf);

}
