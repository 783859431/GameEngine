#include "CommandBuffer.h"
#include "Device.h"
void CommandBufferPool::init()
{
	device = Device::Instance().device;
	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = Device::Instance().indices.graphicsFamily.value();
	Utils::vk_check(vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool));
}

VkCommandBuffer CommandBufferPool::AllocCommand() {

	VkCommandBuffer cammand;
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;

	Utils::vk_check(vkAllocateCommandBuffers(device, &allocInfo, &cammand));

	return cammand;
}

void CommandBufferPool::AllocCommands(int count, VkCommandBuffer* cammands) {


	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = count;

	Utils::vk_check(vkAllocateCommandBuffers(device, &allocInfo, cammands));
}

void CommandBufferPool::FreeCommand(VkCommandBuffer cmd) {
	vkFreeCommandBuffers(device, commandPool, 1, &cmd);
}

void CommandBufferPool::Clean()
{
	vkDestroyCommandPool(device, commandPool, nullptr);

}




void CommandBuffer::Create() {
	command = CommandBufferPool::Instance().AllocCommand();
}

void CommandBuffer::BegainOnce() {

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	vkBeginCommandBuffer(command, &beginInfo);
}
void CommandBuffer::BindPipeLine(VkPipeline pipeline)
{
	vkCmdBindPipeline(command, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
}
void CommandBuffer::BindVertex(VkBuffer* vertexBuffer, int _offset, uint32_t firstBinding, uint32_t bindingCount) {
	VkDeviceSize offset = _offset;
	vkCmdBindVertexBuffers(command, firstBinding, bindingCount, vertexBuffer, &offset);
}
void CommandBuffer::BindIndex(VkBuffer indexBuffer, int _offset) {
	VkDeviceSize offset = _offset;
	vkCmdBindIndexBuffer(command, indexBuffer, offset, VK_INDEX_TYPE_UINT32);
}
void CommandBuffer::BindSets(VkPipelineLayout lay, VkDescriptorSet* sets, int count) {
	vkCmdBindDescriptorSets(command, VK_PIPELINE_BIND_POINT_GRAPHICS, lay, 0, count, sets, 0, nullptr);
}
void CommandBuffer::PushConst(VkPipelineLayout lay, VkShaderStageFlags stage, void* data, uint32_t size, uint32_t offset) {
	vkCmdPushConstants(command, lay, stage, offset, size, data);
}
void CommandBuffer::DrawIndexed(int indexCount) {

	vkCmdDrawIndexed(command, indexCount, 1, 0, 0, 0); //
}
void CommandBuffer::Draw()
{
	vkCmdDraw(command,6,1,0,0);
}
void CommandBuffer::BeginRenderPass(VkRenderPassBeginInfo& info)
{
	vkCmdBeginRenderPass(command, &info, VK_SUBPASS_CONTENTS_INLINE);
}
void CommandBuffer::EndRenderPass() {

	vkCmdEndRenderPass(command);
}

void CommandBuffer::SetViewport(VkViewport* view, uint32_t first, uint32_t count)
{
	vkCmdSetViewport(command, first, count, view);
}
void CommandBuffer::SetScissor(VkRect2D* scissor, uint32_t first, uint32_t count)
{
	vkCmdSetScissor(command, first, count, scissor);
}
void CommandBuffer::Reset(VkCommandBufferResetFlagBits flags) {
	vkResetCommandBuffer(command, flags);
}
void CommandBuffer::EndOnce() {

	vkEndCommandBuffer(command);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &command;

	vkQueueSubmit(Device::Instance().graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(Device::Instance().graphicsQueue);
	CommandBufferPool::Instance().FreeCommand(command);

}
void CommandBuffer::Begain() {

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	if (vkBeginCommandBuffer(command, &beginInfo) != VK_SUCCESS) {
		throw std::runtime_error("failed to begin recording command buffer!");
	}


}
void CommandBuffer::End() {

	if (vkEndCommandBuffer(command) != VK_SUCCESS) {
		throw std::runtime_error("failed to record command buffer!");
	}

}

