#pragma once
#include "vulkan/vulkan.h"
#include "vk_mem_alloc.h"
#include "Allocator.h"
#include <memory>
#include "stb_image.h"
#include "Global.h"
#include "StagingBuffer.h"
#include "Device.h"
class Image
{
	
  
public:  
    AllocatedImage allocImg;
    VkImageView imageView;
    
  //  VkSampler sampler; //Í¼Æ¬²ÉÑùÆ÷
    void LoadRes(std::string path);
    void CreateGpuBuffer();
    void CopyToGpu(char* imgRawData);
    void Clean();
    void ChangeImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
    void CreateImageView(VkFormat format);
    void FreeImageView();
    void CreateEmptyImage(float w, float h);
    VkImage GetImage() {
        return allocImg.img;
    }
    VkImageView GetImageView() {
        return imageView;
    }
    static VkSampler CreateSampler()
    {
        VkSampler sampler;
        VkPhysicalDeviceProperties properties{};
        vkGetPhysicalDeviceProperties(Device::Instance().gpu, &properties);

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

        if (vkCreateSampler(Device::Instance().device, &samplerInfo, nullptr, &sampler) != VK_SUCCESS) {
            throw std::runtime_error("failed to create texture sampler!");
        }
        return sampler;
    }
   


};

