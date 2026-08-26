#include "Allocator.h"
#include "Device.h"

void Allocator::init() {
    VmaAllocatorCreateInfo allocatorInfo = {};
    allocatorInfo.physicalDevice = Device::getInstance().gpu;
    allocatorInfo.device = Device::getInstance().device;
    allocatorInfo.instance = Device::getInstance().ins;
    Utils::vk_check(vmaCreateAllocator(&allocatorInfo, &allocator));
}

void Allocator::clean() {
    if(allocator)
        vmaDestroyAllocator(allocator);
}
/// <summary>
/// 在Vulkan中，内存分配是一个复杂的过程，因为你需要考虑内存类型、内存属性以及性能需求。
/// </summary>
/// <param name="size"> 分配字节数</param>
/// <param name="usage"> buffer的用途例如 VK_BUFFER_USAGE_VERTEX_BUFFER_BIT </param>
/// <param name="allocBuf"> 分配成功后保存相关buffer信息</param>
/// <param name="vma_flag"> 内存属性</param>
/// <param name="vma_usage"> 和内存属性配合使用</param>
void Allocator::allocBuffer(
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
    
    VmaAllocationInfo info;
    Utils::vk_check(vmaCreateBuffer(allocator, &bufferInfo, &vmaallocInfo,
        &allocBuf._buffer,
        &allocBuf._allocation,
        &info));
    if(vma_flag& VMA_ALLOCATION_CREATE_MAPPED_BIT)
      allocBuf.mapped = info.pMappedData;
    else
      allocBuf.mapped = nullptr;//如果没有映射内存，则设置为nullptr
}


//创建图像缓冲区，通常用于存储纹理数据或者帧缓冲附件。图像缓冲区的分配需要考虑图像的格式、尺寸、使用方式以及内存属性等因素。
void Allocator::allocImage(AllocatedImage& imgAlloc) {

    VkImageCreateInfo imgCreateInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
    imgCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    imgCreateInfo.extent.width = imgAlloc.width;
    imgCreateInfo.extent.height = imgAlloc.height;
    imgCreateInfo.extent.depth = 1;
    imgCreateInfo.mipLevels = imgAlloc.mipLevel;
    imgCreateInfo.arrayLayers = 1;
    imgCreateInfo.format = imgAlloc.format;
    imgCreateInfo.tiling = imgAlloc.tiling;
    imgCreateInfo.initialLayout = imgAlloc.imageLayout;
    imgCreateInfo.usage = imgAlloc.usage ;
    imgCreateInfo.samples = imgAlloc.samples;

    VmaAllocationCreateInfo allocCreateInfo = {};
    allocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
    allocCreateInfo.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
    allocCreateInfo.priority = 1.0f;
    Utils::vk_check(vmaCreateImage(allocator, &imgCreateInfo, &allocCreateInfo, &imgAlloc.img, &imgAlloc.allocation, nullptr));
}
//分配顶点缓冲区
void Allocator::allocVertexBuffer(AllocatedBuffer& alloc)
{
    allocBuffer(
        alloc.size,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        alloc,
        VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT,
        VMA_MEMORY_USAGE_CPU_TO_GPU);
}
//分配索引缓冲区
void Allocator::allocIndexBuffer(AllocatedBuffer& alloc) {
    allocBuffer(
        alloc.size,
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        alloc,
        VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT,
        VMA_MEMORY_USAGE_CPU_TO_GPU);
}
//分配临时缓冲区，通常用于数据传输
void Allocator::allocStagingBuffer(AllocatedBuffer& alloc) {

    allocBuffer(
        alloc.size,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        alloc,
        VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT| VMA_ALLOCATION_CREATE_MAPPED_BIT,
        VMA_MEMORY_USAGE_CPU_ONLY
    );

}
//销毁缓冲区
void Allocator::freeBuffer(AllocatedBuffer& buffer) {
    vmaDestroyBuffer(allocator, buffer._buffer, buffer._allocation);
    buffer._allocation = 0;
}

//销毁图像缓冲区
void Allocator::freeImage(VkImage& img, VmaAllocation& alloc) {
    vmaDestroyImage(allocator, img, alloc);
    alloc = 0;
    img = 0;
}
//映射GPU缓冲区内存到CPU地址空间，允许CPU访问缓冲区数据
void Allocator::mapBuffer(AllocatedBuffer& buf)
{
    vmaMapMemory(allocator, buf._allocation, &buf.mapped);
}
//解除映射，释放CPU对缓冲区数据的访问权限
void Allocator::unMapBuffer(AllocatedBuffer& buf) {

    vmaUnmapMemory(allocator, buf._allocation);
}
//分配统一缓冲区，通常用于存储着色器的常量数据，例如变换矩阵、光照参数等。Uniform Buffer需要满足特定的对齐要求，因此在分配时需要考虑这些要求。
void Allocator::allocUniformBuffer(AllocatedBuffer& alloc) {
    allocBuffer(
        alloc.size,
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        alloc,
        VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT,
        VMA_MEMORY_USAGE_CPU_TO_GPU);


}

void Allocator::allocStorageBuffer(AllocatedBuffer& alloc)
{
    allocBuffer(
        alloc.size,
        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
        alloc,
        VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT,
        VMA_MEMORY_USAGE_CPU_TO_GPU);

}

VmaAllocator Allocator::allocator = nullptr;
