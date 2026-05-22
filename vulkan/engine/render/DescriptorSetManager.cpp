#include "DescriptorSetManager.h"
#include "Device.h"
// 先在的问题是每个物体需要独立的一块内存来存放uniform 矩阵，如果直接在同一块内存上放uniform矩阵，
// 无法做到gpu完成第一个物体的绘制后更新uniform矩阵再绘制新的物体。

std::vector<VkDescriptorPoolSize> DescriptorSetPool::poolSizes = {
		{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,MAX_IMAGE_SAMPlER},
		{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,MAX_UNIFORM_BUFFER}
};

VkDescriptorPool DescriptorSetPool::setsPool = 0;

void DescriptorSetPool::init(int maxSets , VkDescriptorPoolCreateFlags flags ) {

	VkDescriptorPoolCreateInfo descriptorPoolInfo{};
	descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	descriptorPoolInfo.pPoolSizes = poolSizes.data();
	descriptorPoolInfo.maxSets = maxSets;
	descriptorPoolInfo.flags = flags;
	if (vkCreateDescriptorPool(Device::getInstance().device, &descriptorPoolInfo, nullptr, &setsPool) !=
		VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool!");
	}
}




//set0 set1 set2 
void DescriptorSetPool::allocSets(uint32_t setsCount, VkDescriptorSetLayout* setLayouts, VkDescriptorSet* descSets)
{
	///////////////////////////
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = setsPool;
	allocInfo.pSetLayouts = setLayouts;
	allocInfo.descriptorSetCount = setsCount;
	if (vkAllocateDescriptorSets(Device::getInstance().device, &allocInfo, descSets) != VK_SUCCESS) {
		throw std::runtime_error("failed to create set");
	}

}


void DescriptorSetPool::clean() {
	vkDestroyDescriptorPool(Device::getInstance().device, setsPool, nullptr);
}
