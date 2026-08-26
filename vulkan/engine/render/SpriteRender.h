#pragma once
#include "Pipeline.h"
#include "Sprite.h"
#include "Texture.h"
#include "StorageBuffer.h"
struct InstanceData
{
	glm::vec4 transform;
	Rect rect; // 决定了采样位置
	glm::vec4 sheetWH;// 采样纹理的长宽
};

class SpriteRender
{
	Pipeline spritePipeline;
	StorageBuffer sbuf[MAX_FRAMES_IN_FLIGHT];
	VkDescriptorSet sets[MAX_FRAMES_IN_FLIGHT];
	std::unordered_map<Texture*, std::vector<InstanceData>> diffTex;
	DSetLayout setLayout;
	DSetLayout setLayout2;
	void createDescriptorSet();
	
public:
	~SpriteRender();


	void init(VkRenderPass pass);

	void createPipeline(VkRenderPass pass);

	void updateTexture(CommandBuffer& cmd, Texture* tex);

	void drawSprite(Sprite* sp);

	void flush(CommandBuffer& cmd,int  frame);

	void clean();



};

