#pragma once
#include <vulkan/vulkan.h>
#include <unordered_map>
#include <vector>
#include <stdexcept>
#include <memory>
#include "Device.h"
#define MAX_IMAGE_SAMPlER  (1<<3)
#define MAX_UNIFORM_BUFFER (1<<3)

class WriteSetHelper {

	std::vector<VkWriteDescriptorSet> writeSets;
public:
	WriteSetHelper& AddWriteBuffer(
		uint32_t binding,
		VkDescriptorType  descType,
		VkDescriptorSet set,
		VkDescriptorBufferInfo* bufferInfo
		)
	{
		VkWriteDescriptorSet write{};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.descriptorType = descType;
		write.dstBinding = binding;
		write.pBufferInfo = bufferInfo;
		write.descriptorCount = 1;
		write.dstSet = set;
        writeSets.push_back(write);
		return *this;

	}

	WriteSetHelper& AddWriteImage(
		uint32_t binding,
		VkDescriptorType  descType,
		VkDescriptorSet set,
		VkDescriptorImageInfo* imageInfo,
	    uint32_t count,
		uint32_t index = 0)
	{
		VkWriteDescriptorSet write{};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.descriptorType = descType;
		write.dstBinding = binding;
		write.dstArrayElement = index;
		write.pImageInfo = imageInfo;
		write.descriptorCount = count;
		write.dstSet = set;
		writeSets.push_back(write);
		return *this;
	}
	void Update() {
		
		vkUpdateDescriptorSets(Device::getInstance().device, writeSets.size(), writeSets.data(), 0, nullptr);
	}

};





class DescriptorSetPool
{

	
public:

	static VkDescriptorPool setsPool ;
	static std::vector<VkDescriptorPoolSize> poolSizes;
	
	static void init(int maxSets = 20, VkDescriptorPoolCreateFlags flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT);

	//set0 set1 set2 
	static void allocSets(uint32_t setsCount, VkDescriptorSetLayout* setLayouts, VkDescriptorSet* descSets);

	static void clean();

};




class DescriptorSetLayoutBuilder
{
	std::vector<VkDescriptorSetLayoutBinding> bindings;

public:

	DescriptorSetLayoutBuilder& AddBinding(
		uint32_t binding,
		VkDescriptorType descriptorType,
		VkShaderStageFlags stageFlags,
		uint32_t count) {
		VkDescriptorSetLayoutBinding layoutBinding{};
		layoutBinding.binding = binding;
		layoutBinding.descriptorType = descriptorType;
		layoutBinding.descriptorCount = count;
		layoutBinding.stageFlags = stageFlags;
		bindings.push_back(layoutBinding);
		return *this;
	}
	VkDescriptorSetLayout build() 
	{
		VkDescriptorSetLayout descriptorSetLayout;
		VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
		descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		descriptorSetLayoutInfo.pBindings = bindings.data();

		if (vkCreateDescriptorSetLayout(
			Device::getInstance().device,
			&descriptorSetLayoutInfo,
			nullptr,
			&descriptorSetLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor set layout!");
		}
		return descriptorSetLayout;
	}

};




