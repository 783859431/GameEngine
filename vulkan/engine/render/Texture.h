#pragma once
#include "vulkan/vulkan.h"
#include "vk_mem_alloc.h"
#include "Allocator.h"
#include <memory>
#include "stb_image.h"
#include "StagingBuffer.h"
#include "Device.h"
/// <summary>
/// 从文件中加载图片
/// 将图片上传GPU
/// 
/// </summary>

class Texture
{
	    //获取字节大小

    int getPixelSize();
    int getImageSize();
public: 

    VmaAllocation allocation = nullptr;
    VkImage img = VK_NULL_HANDLE;
    VkImageView imageView = VK_NULL_HANDLE;
    VkSampler sampler = VK_NULL_HANDLE;
    int height = 0;
    int width = 0;
    uint32_t mipLevels = 1;
    VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;
    VkImageLayout imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    // 创建空白纹理（用于渲染目标）
    void createEmpty(
        uint32_t width, 
        uint32_t height,
        VkImageUsageFlags usage= VK_IMAGE_USAGE_SAMPLED_BIT,
        VkFormat format= VK_FORMAT_R8G8B8A8_UNORM,  
        VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL);
    static VkSampler getDefaultSampler();
    static void destroyDefaultSampler();
    VkDescriptorImageInfo getDescriptorInfo() const {
        VkDescriptorImageInfo info{};
        info.imageLayout = imageLayout;
        info.imageView = imageView;
        info.sampler = sampler;
        return info;
    }
    //从文件中读取图片到GPU
    void loadRes(const std::string& path);
    //将数据拷贝到分配好的内存中
    void copyToGpu(char* imgRawData,int size);
    void clean();
    //修改imageLayout
    void changeImageLayout(VkImage image,  VkImageLayout oldLayout, VkImageLayout newLayout);
    //创建ImageView
    static VkImageView CreateImageView(VkImage img, VkFormat format, VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D, uint32_t mipLevel = 1);
    //获取描述
    VkDescriptorImageInfo getDescriptorImageInfo()
    {
        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout =this->imageLayout;
        imageInfo.imageView = this->imageView;
        imageInfo.sampler = this->sampler; 

        return imageInfo;
    }
    ~Texture()
    {
        clean();
    }
    VkImage getImage() {
        return img;
    }

    VkImageView getImageView() {
        return imageView;
    }
    void createImageView(VkImageViewType tp = VK_IMAGE_VIEW_TYPE_2D);

    static VkSampler createSampler()
    {
        VkSampler sampler;
        VkPhysicalDeviceProperties properties{};
        vkGetPhysicalDeviceProperties(Device::getInstance().gpu, &properties);

        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.anisotropyEnable = VK_FALSE;
        samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

        if (vkCreateSampler(Device::getInstance().device, &samplerInfo, nullptr, &sampler) != VK_SUCCESS) {
            throw std::runtime_error("failed to create texture sampler!");
        }
        return sampler;
    }
   


};

