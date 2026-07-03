#pragma once
#include "Pipeline.h"
#include "Sprite.h"
#include "Texture.h"
#include "StorageBuffer.h"
struct InstanceData
{
	glm::mat4 model;
	Rect rect; // 决定了采样位置
	glm::vec4 sheetWH;// 采样纹理的长宽
};

class SpriteRender
{
	Pipeline spritePipeline;
	StorageBuffer sbuf;
	std::unordered_map<Texture*, std::vector<InstanceData>> diffTex;
	DSetLayout setLayout;
	DSetLayout layout2;
	void createDescriptorSet();

public:

	VkDescriptorSet sets[2];

	void init(VkRenderPass pass, VkDescriptorSetLayout setLayout0);

	void createPipeline(VkRenderPass pass, VkDescriptorSetLayout setLayout);

	void updateTexture(CommandBuffer& cmd, Texture* tex);

	void drawSprite(Sprite* sp);

	void flush(CommandBuffer& cmd);


};

