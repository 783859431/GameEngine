#pragma once
#include <vulkan/vulkan.h>
#include <unordered_map>
#include <vector>
#include <stdexcept>
#include <memory>
#include "Device.h"
#include <unordered_map>


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


//TODO:统计每种资源的数量

struct DSetLayout {

	VkDescriptorSetLayout layout;
	std::unordered_map<VkDescriptorType, uint32_t> layoutInfo;
};



class DPool {

	struct Usage
	{
		uint32_t total = 0;
		uint32_t usage = 0;
		uint32_t remain = 0;
	};
public:
	DPool() {

	}

	VkDescriptorPool setsPool = 0;

	std::unordered_map<VkDescriptorType, Usage> poolInfo;
	//当前pool的最大的set
	Usage setUsage = {0}; 
	
	void allocSets(std::vector<DSetLayout*>& layouts, std::vector<VkDescriptorSet>& sets);
	void _allocSets(DSetLayout** setLayouts,uint32_t count, VkDescriptorSet* sets);
	void clean();
};

class PoolManager
{
private:
	PoolManager() = default;
public :
	/*
	static Device& getInstance() {
		static Device instance;
		return instance;
	}
	*/

	static PoolManager& inst() {
	    static PoolManager _inst;
		return _inst;
	}
	void init();
	std::vector< std::unique_ptr<DPool>> pools;
	void allocSets(std::vector<DSetLayout*>& layouts, std::vector<VkDescriptorSet>& outSets);
	void allocSets(DSetLayout** layouts, int count, VkDescriptorSet* outsets);
	void allocSet(DSetLayout& layout, VkDescriptorSet* outset);
	int createPool(std::vector<VkDescriptorPoolSize>& poolSize, uint32_t maxSets);
	int findOrCreatePool(std::vector<DSetLayout*>& layouts);
	void clean();
};


class DescriptorSetLayoutBuilder
{
	std::vector<VkDescriptorSetLayoutBinding> bindings;
	std::vector<VkDescriptorBindingFlags> flags;
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
	DescriptorSetLayoutBuilder& AddFlag(VkDescriptorBindingFlags flag) {
		flags.push_back(flag);
		return *this;
	}
	DSetLayout build() 
	{
		VkDescriptorSetLayout descriptorSetLayout;
		VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
		descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		descriptorSetLayoutInfo.pBindings = bindings.data();
		VkDescriptorBindingFlags bindingFlag = 0;
		VkDescriptorSetLayoutBindingFlagsCreateInfo bindingFlags{};
		bindingFlags.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
		if (flags.size() > 0)
		{
			 // VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT | VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT;
			for (auto f : flags)
			{
				bindingFlag |= f;
			}
			bindingFlags.bindingCount = 1;
			bindingFlags.pBindingFlags = &bindingFlag;
			descriptorSetLayoutInfo.pNext = &bindingFlags;
		}
	
		if (vkCreateDescriptorSetLayout(
			Device::getInstance().device,
			&descriptorSetLayoutInfo,
			nullptr,
			&descriptorSetLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor set layout!");
		}
		DSetLayout layout;
		for (int i = 0; i < bindings.size(); i++)
		{
			layout.layoutInfo[bindings[i].descriptorType] = bindings[i].descriptorCount;
		}
		layout.layout = descriptorSetLayout;
		flags.clear();
		bindings.clear();
		return  layout;
	}

};




