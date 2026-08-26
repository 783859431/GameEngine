#pragma once
#include "vk_mem_alloc.h"
struct AllocatedBuffer {
    VkBuffer _buffer = VK_NULL_HANDLE;
    VmaAllocation _allocation =VK_NULL_HANDLE;
    uint32_t size =0;
    void* mapped = nullptr;
};

struct AllocatedImage {
    VkImage img = VK_NULL_HANDLE;
    VmaAllocation allocation = VK_NULL_HANDLE;
    int width = 0;
    int height = 0;
    uint32_t size = 0; // bytes;
    VkFormat format = VK_FORMAT_UNDEFINED;
    VkImageTiling tiling = VK_IMAGE_TILING_LINEAR;
    VkImageUsageFlags usage = VK_IMAGE_USAGE_SAMPLED_BIT;
    VkImageLayout imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    uint32_t mipLevel = 1;
    VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT;
};



class Allocator
{

	 static VmaAllocator allocator;
public:
    static void init();
    static void clean();
    static void allocBuffer(
        VkDeviceSize size,
        VkBufferUsageFlags usage,
        AllocatedBuffer& allocBuf,
        VmaAllocationCreateFlags vma_flag,
        VmaMemoryUsage vma_usage = VMA_MEMORY_USAGE_AUTO);
    static void allocImage(AllocatedImage& alloc );
    static void allocVertexBuffer(AllocatedBuffer& alloc);
    static void allocIndexBuffer(AllocatedBuffer& alloc);
    static void allocStagingBuffer(AllocatedBuffer& alloc);
    static void freeBuffer(AllocatedBuffer& buffer);
    static void freeImage(VkImage& image, VmaAllocation& alloc);
    static void mapBuffer(AllocatedBuffer& buf);
    static void unMapBuffer(AllocatedBuffer& buf);
    static void allocUniformBuffer(AllocatedBuffer& alloc);
    static void allocStorageBuffer(AllocatedBuffer& alloc);


};

