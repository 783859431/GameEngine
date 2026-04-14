#pragma once
#include "vulkan/vulkan.h"
#include "vk_mem_alloc.h"
#include "Allocator.h"
#include <memory>
#include "CommandBuffer.h"
class StagingBuffer
{

	AllocatedBuffer allocBuf;
public :
	void AllocBuffer(uint32_t size);

	void CopyTempData(char* data);

	void Clean();

	void CopyToImage(AllocatedImage& dst, bool freeAfterCopy = true);


	void CopyToBuffer(AllocatedBuffer& dst, bool freeAfterCopy = true);



};

