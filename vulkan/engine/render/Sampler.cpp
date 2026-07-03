#include "Sampler.h"
#include "Device.h"
static VkSampler lr = 0,lc = 0,nr =0;

void SamplerManager::createDefaultSamplers()
{

    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(Device::getInstance().gpu, &properties);

    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    //当图像被放大或者缩小时
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    //纹理坐标超过标准范围时如何表现
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    //在不同的方向上使用不同的采样率
    samplerInfo.anisotropyEnable = VK_FALSE;
    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;

    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    //深度比较
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    //Mipmap
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

    if (vkCreateSampler(Device::getInstance().device, &samplerInfo, nullptr, &lr) != VK_SUCCESS) {
        throw std::runtime_error("failed to create lr sampler!");
    }
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    if (vkCreateSampler(Device::getInstance().device, &samplerInfo, nullptr, &lc) != VK_SUCCESS) {
        throw std::runtime_error("failed to create lc sampler!");
    }
    samplerInfo.magFilter = VK_FILTER_NEAREST;
    samplerInfo.minFilter = VK_FILTER_NEAREST;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    if (vkCreateSampler(Device::getInstance().device, &samplerInfo, nullptr, &nr) != VK_SUCCESS) {
        throw std::runtime_error("failed to create nr sampler!");
    }
}

void SamplerManager::clean()
{
    vkDestroySampler(Device::getInstance().device, lr, 0);
    vkDestroySampler(Device::getInstance().device, lc, 0);
    vkDestroySampler(Device::getInstance().device, nr, 0);

}

VkSampler SamplerManager::getSamper(SamplerType tp)
{
    if (tp == SamplerType::LinearRepeat)
    {
        return lr;
    }
    else if (tp == SamplerType::LinearClamp)
    {
        return lc;
    }
    return nr;

}