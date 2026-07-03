#include "CommandBuffer.h"
#include "Device.h"
void CommandBufferPool::init()
{

	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = Device::getInstance().indices.graphicsFamily.value();
	Utils::vk_check(vkCreateCommandPool(Device::getInstance().device, &poolInfo, nullptr, &commandPool));
}

VkCommandBuffer CommandBufferPool::allocCommand(VkCommandBufferLevel level) {

	VkCommandBuffer cammand;
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = level;
	allocInfo.commandBufferCount = 1;
	allocInfo.pNext = nullptr;
	Utils::vk_check(vkAllocateCommandBuffers(Device::getInstance().device, &allocInfo, &cammand));

	return cammand;
}

void CommandBufferPool::allocCommands(int count, VkCommandBuffer* cammands , VkCommandBufferLevel level) {


	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = level;
	allocInfo.commandBufferCount = count;

	Utils::vk_check(vkAllocateCommandBuffers(Device::getInstance().device, &allocInfo, cammands));
}

void CommandBufferPool::freeCommand(VkCommandBuffer cmd) {
	vkFreeCommandBuffers(Device::getInstance().device, commandPool, 1, &cmd);
}

void CommandBufferPool::clean()
{
	if(commandPool)
		vkDestroyCommandPool(Device::getInstance().device, commandPool, nullptr);

}


void CommandBuffer::create(VkCommandBufferLevel level) {

	m_command = CommandBufferPool::allocCommand(level);
}

void CommandBuffer::begainOnce() {

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	vkBeginCommandBuffer(m_command, &beginInfo);
}
void CommandBuffer::bindPipeLine(VkPipeline pipeline)
{
	vkCmdBindPipeline(m_command, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
}
void CommandBuffer::bindVertex(VkBuffer vertexBuffer, int _offset, uint32_t firstBinding, uint32_t bindingCount) {
	VkDeviceSize offset = _offset;
	vkCmdBindVertexBuffers(m_command, firstBinding, bindingCount, &vertexBuffer, &offset);
}
void CommandBuffer::bindIndex(VkBuffer indexBuffer, int _offset) {
	VkDeviceSize offset = _offset;
	vkCmdBindIndexBuffer(m_command, indexBuffer, offset, VK_INDEX_TYPE_UINT32);
}
void CommandBuffer::bindSets(VkPipelineLayout lay, VkDescriptorSet* sets, uint32_t count , uint32_t first,uint32_t dynamicCount,uint32_t* dynamicOffset ) {
	vkCmdBindDescriptorSets(m_command, VK_PIPELINE_BIND_POINT_GRAPHICS, lay, first, count, sets, dynamicCount, dynamicOffset);
}

void CommandBuffer::pushConst(VkPipelineLayout lay, VkShaderStageFlags stage, void* data, uint32_t size, uint32_t offset) {
	vkCmdPushConstants(m_command, lay, stage, offset, size, data);
}
void CommandBuffer::drawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex,int offset,  uint32_t firstInstance) {

	vkCmdDrawIndexed(m_command, indexCount, instanceCount, firstIndex, offset, firstInstance); //
}
void CommandBuffer::draw(int vertexCount , int instCount ,int firstVertex ,int firstIns)
{
	vkCmdDraw(m_command,vertexCount,instCount,firstVertex,firstIns);
}
void CommandBuffer::beginRenderPass(VkRenderPassBeginInfo& info)
{
	vkCmdBeginRenderPass(m_command, &info, VK_SUBPASS_CONTENTS_INLINE);
}
void CommandBuffer::endRenderPass() {

	vkCmdEndRenderPass(m_command);
}

void CommandBuffer::setViewport(VkViewport* view, uint32_t first, uint32_t count)
{
	vkCmdSetViewport(m_command, first, count, view);
}
void CommandBuffer::setScissor(VkRect2D* scissor, uint32_t first, uint32_t count)
{
	vkCmdSetScissor(m_command, first, count, scissor);
}
void CommandBuffer::reset(VkCommandBufferResetFlagBits flags) {
	vkResetCommandBuffer(m_command, flags);
}
void CommandBuffer::endOnce() {

	vkEndCommandBuffer(m_command);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &m_command;

	vkQueueSubmit(Device::getInstance().graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(Device::getInstance().graphicsQueue);
	//CommandBufferPool::freeCommand(m_command);

}
void CommandBuffer::begain() {

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	if (vkBeginCommandBuffer(m_command, &beginInfo) != VK_SUCCESS) {
		throw std::runtime_error("failed to begin recording m_command buffer!");
	}


}
void CommandBuffer::end() {

	if (vkEndCommandBuffer(m_command) != VK_SUCCESS) {
		throw std::runtime_error("failed to record m_command buffer!");
	}

}

VkCommandPool CommandBufferPool::commandPool = nullptr;
