#pragma once
#include "vk_mem_alloc.h"
#include "Utils.h"
#include "singleton.h"
#include "Global.h"
struct AllocatedBuffer {
    VkBuffer _buffer;
    VmaAllocation _allocation;
    uint32_t size;
    void* mapped;
};
struct AllocatedImage {
    VkImage img;
    VmaAllocation allocation;
    int width, height;
    uint32_t size; // bytes;
};
class Allocator: public Singleton<Allocator>
{
    friend class Singleton<Allocator>;
	VmaAllocator allocator;
public:

    void init();

    void Clean();
    void AllocBuffer(
        VkDeviceSize size,
        VkBufferUsageFlags usage,
        AllocatedBuffer& allocBuf,
        VmaAllocationCreateFlags vma_flag,
        VmaMemoryUsage vma_usage = VMA_MEMORY_USAGE_AUTO);
    void AllocImage(AllocatedImage& imgAlloc);
    void AllocVertexBuffer(AllocatedBuffer& alloc);
    void AllocIndexBuffer(AllocatedBuffer& alloc);
    void AllocStagingBuffer(AllocatedBuffer& alloc);

    void FreeBuffer(AllocatedBuffer& buffer);
    void FreeImage(AllocatedImage& image);
    void MapBuffer(AllocatedBuffer& buf);
    void UnMapBuffer(AllocatedBuffer& buf);
    void AllocUniformBuffer(AllocatedBuffer& alloc);
};

