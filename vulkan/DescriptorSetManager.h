#pragma once
#include <vulkan/vulkan.h>
#include <unordered_map>
#include <vector>
#include <stdexcept>
#include <memory>
#include "Global.h"
#include "singleton.h"
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
		
		vkUpdateDescriptorSets(Device::Instance().device, writeSets.size(), writeSets.data(), 0, nullptr);
	}

};





class DescriptorSetPool:public Singleton<DescriptorSetPool>
{
	friend class Singleton<DescriptorSetPool>;
	
public:

	VkDescriptorPool setsPool = nullptr;
	std::vector<VkDescriptorPoolSize> poolSizes = {
		{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,MAX_IMAGE_SAMPlER},
		{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,MAX_UNIFORM_BUFFER}
	};
	
	void init(int maxSets = 5, VkDescriptorPoolCreateFlags flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT);

	//set0 set1 set2 
	void AllocSets(uint32_t setsCount, VkDescriptorSetLayout* setLayouts, VkDescriptorSet* descSets);

	void Clean();

};




class DescriptorSetLayoutBuilder
{
	std::vector<VkDescriptorSetLayoutBinding> bindings;
	VkDevice device;
public:
	DescriptorSetLayoutBuilder& SetDevice(VkDevice device) {
		this->device = device;
		return *this;
	}
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
			device,
			&descriptorSetLayoutInfo,
			nullptr,
			&descriptorSetLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor set layout!");
		}
		return descriptorSetLayout;
	}

};




