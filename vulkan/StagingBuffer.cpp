#include "StagingBuffer.h"
void StagingBuffer:: AllocBuffer(uint32_t size)
{
	allocBuf.size = size;
	Allocator::Instance().AllocStagingBuffer(allocBuf);
	Allocator::Instance().MapBuffer(allocBuf); // GPU can access this memory 
}

void StagingBuffer::CopyTempData(char* data)
{
	memcpy(allocBuf.mapped, data, allocBuf.size);
}

void StagingBuffer::Clean() {

	Allocator::Instance().UnMapBuffer(allocBuf);
	Allocator::Instance().FreeBuffer(allocBuf);
}


void StagingBuffer::CopyToImage(AllocatedImage& dst, bool freeAfterCopy)
{

	CommandBuffer commadBuffer;
	commadBuffer.Create();
	commadBuffer.BegainOnce();
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
		(uint32_t)dst.width,
		(uint32_t)dst.height,
		1
	};

	vkCmdCopyBufferToImage(commadBuffer.command, allocBuf._buffer, dst.img, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
	commadBuffer.EndOnce();
	if (freeAfterCopy)
		Clean();

}


void StagingBuffer::CopyToBuffer(AllocatedBuffer& dst, bool freeAfterCopy)
{
	VkBufferCopy region;
	region.dstOffset = 0;
	region.srcOffset = 0;
	region.size = dst.size;

	CommandBuffer commadBuffer;
	commadBuffer.Create();
	commadBuffer.BegainOnce();
	vkCmdCopyBuffer(commadBuffer.command, allocBuf._buffer, dst._buffer, 1, &region);
	commadBuffer.EndOnce();
	if (freeAfterCopy)
		Clean();

}
