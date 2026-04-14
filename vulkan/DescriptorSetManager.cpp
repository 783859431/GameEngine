#include "DescriptorSetManager.h"
#include "Device.h"
void DescriptorSetPool::init(int maxSets , VkDescriptorPoolCreateFlags flags ) {

	VkDescriptorPoolCreateInfo descriptorPoolInfo{};
	descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	descriptorPoolInfo.pPoolSizes = poolSizes.data();
	descriptorPoolInfo.maxSets = maxSets;
	descriptorPoolInfo.flags = flags;
	if (vkCreateDescriptorPool(Device::Instance().device, &descriptorPoolInfo, nullptr, &setsPool) !=
		VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool!");
	}
}




//set0 set1 set2 
void DescriptorSetPool::AllocSets(uint32_t setsCount, VkDescriptorSetLayout* setLayouts, VkDescriptorSet* descSets)
{
	///////////////////////////
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = setsPool;
	allocInfo.pSetLayouts = setLayouts;
	allocInfo.descriptorSetCount = setsCount;
	if (vkAllocateDescriptorSets(Device::Instance().device, &allocInfo, descSets) != VK_SUCCESS) {
		throw std::runtime_error("failed to create set");
	}

}


void DescriptorSetPool::Clean() {
	vkDestroyDescriptorPool(Device::Instance().device, setsPool, nullptr);
}
