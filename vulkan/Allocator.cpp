#include "Allocator.h"
#include "Device.h"
//œ‘¥Ê∑÷≈‰∆˜
void Allocator::init() {
    VmaAllocatorCreateInfo allocatorInfo = {};
    allocatorInfo.physicalDevice = Device::Instance().gpu;
    allocatorInfo.device = Device::Instance().device;
    allocatorInfo.instance = Device::Instance().ins;
    Utils::vk_check(vmaCreateAllocator(&allocatorInfo, &allocator));
}

void Allocator::Clean() {
    vmaDestroyAllocator(allocator);
}
void Allocator::AllocBuffer(
    VkDeviceSize size,
    VkBufferUsageFlags usage,
    AllocatedBuffer& allocBuf,
    VmaAllocationCreateFlags vma_flag,
    VmaMemoryUsage vma_usage) {
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo vmaallocInfo = {};
    vmaallocInfo.usage = vma_usage;
    vmaallocInfo.flags = vma_flag;
    //allocate the buffer
    Utils::vk_check(vmaCreateBuffer(allocator, &bufferInfo, &vmaallocInfo,
        &allocBuf._buffer,
        &allocBuf._allocation,
        nullptr));
}
void Allocator::AllocImage(AllocatedImage& imgAlloc) {

    VkImageCreateInfo imgCreateInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
    imgCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    imgCreateInfo.extent.width = imgAlloc.width;
    imgCreateInfo.extent.height = imgAlloc.height;
    imgCreateInfo.extent.depth = 1;
    imgCreateInfo.mipLevels = 1;
    imgCreateInfo.arrayLayers = 1;
    imgCreateInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
    imgCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imgCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imgCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    imgCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;

    VmaAllocationCreateInfo allocCreateInfo = {};
    allocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
    allocCreateInfo.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
    allocCreateInfo.priority = 1.0f;
    Utils::vk_check(vmaCreateImage(allocator, &imgCreateInfo, &allocCreateInfo, &imgAlloc.img, &imgAlloc.allocation, nullptr));
}
void Allocator::AllocVertexBuffer(AllocatedBuffer& alloc)
{
    AllocBuffer(
        alloc.size,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        alloc,
        VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
        VMA_MEMORY_USAGE_AUTO_PREFER_HOST);
}
void Allocator::AllocIndexBuffer(AllocatedBuffer& alloc) {
    AllocBuffer(
        alloc.size,
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        alloc,
        VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
        VMA_MEMORY_USAGE_AUTO_PREFER_HOST);
}
void Allocator::AllocStagingBuffer(AllocatedBuffer& alloc) {

    AllocBuffer(
        alloc.size,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        alloc,
        VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
    );

}

void Allocator::FreeBuffer(AllocatedBuffer& buffer) {
    vmaDestroyBuffer(allocator, buffer._buffer, buffer._allocation);
}
void Allocator::FreeImage(AllocatedImage& image) {
    vmaDestroyImage(allocator, image.img, image.allocation);

}
void Allocator::MapBuffer(AllocatedBuffer& buf)
{
    vmaMapMemory(allocator, buf._allocation, &buf.mapped);
}
void Allocator::UnMapBuffer(AllocatedBuffer& buf) {

    vmaUnmapMemory(allocator, buf._allocation);
}
void Allocator::AllocUniformBuffer(AllocatedBuffer& alloc) {
    AllocBuffer(
        alloc.size,
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        alloc,
        VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
        VMA_MEMORY_USAGE_AUTO_PREFER_HOST);


}