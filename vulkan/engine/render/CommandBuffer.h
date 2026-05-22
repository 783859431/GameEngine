#pragma once
#include "vulkan/vulkan.h"

class CommandBufferPool
{

	static VkCommandPool commandPool;
public:	
	static void init();
	static VkCommandBuffer allocCommand(VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
	static void allocCommands(int count, VkCommandBuffer* cammands, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
	static void freeCommand(VkCommandBuffer cmd);
	static void clean();
};

class CommandBuffer
{

public:

	VkCommandBuffer m_command = VK_NULL_HANDLE;
    //从pool中分配一个commandBuffer 
	void create(VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);

	void begainOnce();
	//绑定管线
	void bindPipeLine(VkPipeline pipeline);
	//绑定顶点缓冲区
	void bindVertex(VkBuffer vertexBuffer, int _offset = 0, uint32_t firstBinding = 0, uint32_t bindingCount = 1);
	//绑定索引缓冲区
	void bindIndex(VkBuffer indexBuffer, int _offset = 0);
	//绑定描述符集
	void bindSets(VkPipelineLayout lay, VkDescriptorSet* sets, int count, int first =0);
	//更新常量数据
	void pushConst(VkPipelineLayout lay, VkShaderStageFlags stage, void* data, uint32_t size, uint32_t offset = 0);
	void drawIndexed(int indexCount);
	void draw(int vertexCount, int instCount = 1, int firstVertex = 0, int firstIns = 0);
	void beginRenderPass(VkRenderPassBeginInfo& info);
	void endRenderPass();
	//设置视图大小
	void setViewport(VkViewport* view, uint32_t first = 0, uint32_t count = 1);
	//设置视图裁剪区域
	void setScissor(VkRect2D* scissor, uint32_t first = 0, uint32_t count = 1);
	void reset(VkCommandBufferResetFlagBits flags = (VkCommandBufferResetFlagBits)0);
	void endOnce();
	void begain();
	void end();
	~CommandBuffer()
	{
		if(m_command)
			CommandBufferPool::freeCommand(m_command);
	}
};

