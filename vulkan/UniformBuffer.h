#pragma once
#include "vulkan/vulkan.h"
#include "vk_mem_alloc.h"
#include "Allocator.h"
#include <memory>
class UniformBuffer
{

	AllocatedBuffer allocBuf;
	VkDescriptorType tp = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
public:
	
	void AllocBuffer(uint32_t size);
	void UpdateData(void* uniformData);
	VkBuffer GetBuffer();
	void Clean();
};

