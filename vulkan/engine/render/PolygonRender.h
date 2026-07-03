#pragma once
#include "vulkan/vulkan.h"
#include "Pipeline.h"
#include "CommandBuffer.h"
#include "VertexBuffer.h"
#include "UniformBuffer.h"
struct Transform2d {
	float x;
	float y;
	float c = 1.0f; //cos θ
	float s = 0.0f; //sin 
	float rot; // 旋转角度，而非弧度

};

struct Point
{
	float x;
	float y;
};
struct Circle
{
	Point c;
	float rad;
	float thickness;

};
struct CircleDrawData
{
	Circle circle;
	Transform2d transform;
};

struct PolygonDrawData {

	std::vector<Point> points;
	Transform2d transform;
};

class PolygonRender
{
	int maxVertCount = 1000;
	int maxCircleCount = 100;
	Pipeline pipeline;
	Pipeline pipeline_cir;
	VertexBuffer vtx;
	UniformBuffer ubo;

	VkDescriptorSet set_cir;
	DSetLayout setLayout_cir;
	void createDescriptorSet();
public:
	std::vector<PolygonDrawData> polys;
	std::vector<CircleDrawData> cirs;
	CommandBuffer* cmd;
	void init(VkDescriptorSetLayout layout,VkRenderPass pass);
	void setLineWidth(float w=2);
	void createPipeline(VkRenderPass pass,VkDescriptorSetLayout layout);
	void drawPolygon( Point* points,int count, Transform2d& trans);
	void drawCircle(float cx,float cy, float rad , float thickness, Transform2d& trans);
	void flushPloy();
	void flushCircle();
};

