#pragma once
#include "vulkan/vulkan.h"
#include "singleton.h"
#include "Utils.h"
#include "Global.h"
class CommandBufferPool:public Singleton<CommandBufferPool>
{

	friend class Singleton<CommandBufferPool>;
	VkCommandPool commandPool;
	VkDevice device;
public:	
	void init();
	VkCommandBuffer AllocCommand();
	void AllocCommands(int count, VkCommandBuffer* cammands);
	void FreeCommand(VkCommandBuffer cmd);
	void Clean();
};

class CommandBuffer
{

public:

	VkCommandBuffer command;
	void Create();
	void BegainOnce();
	void BindPipeLine(VkPipeline pipeline);
	void BindVertex(VkBuffer* vertexBuffer, int _offset = 0, uint32_t firstBinding = 0, uint32_t bindingCount = 1);
	void BindIndex(VkBuffer indexBuffer, int _offset = 0);
	void BindSets(VkPipelineLayout lay, VkDescriptorSet* sets, int count);
	void PushConst(VkPipelineLayout lay, VkShaderStageFlags stage, void* data, uint32_t size, uint32_t offset = 0);
	void DrawIndexed(int indexCount);
	void Draw();
	void BeginRenderPass(VkRenderPassBeginInfo& info);
	void EndRenderPass();
	void SetViewport(VkViewport* view, uint32_t first = 0, uint32_t count = 1);
	void SetScissor(VkRect2D* scissor, uint32_t first = 0, uint32_t count = 1);
	void Reset(VkCommandBufferResetFlagBits flags = (VkCommandBufferResetFlagBits)0);
	void EndOnce();
	void Begain();
	void End();

};

