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
	float rad;
	float thickness;
	Transform2d transform;
};

struct PolygonDrawData {

	std::vector<Point> points;
	Transform2d transform;
};

/*layout(location = 0) in vec4 v_instanceTransform; //
layout(location = 1) in vec4 v_instancePoints12;  ////多边形的点
layout(location = 2) in vec4 v_instancePoints34;  //
layout(location = 3) in vec4 v_instancePoints56;  //
layout(location = 4) in vec4 v_instancePoints78;  //
layout(location = 5) in int v_instanceCount;      ////多边形的顶点数量要小于等于八个
layout(location = 6) in float v_instanceRadius;   //
layout(location = 7) in vec4 v_instanceColor; */

struct PolygenInstance {
	glm::vec4 transform;
	glm::vec4 point12, point34, point56, point78;
	glm::vec4 color;
	int pointsCount;
	float  rad;

	static std::vector<VkVertexInputBindingDescription> getBinding()
	{

		VkVertexInputBindingDescription vibd;
		vibd.binding = 0;
		vibd.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;
		vibd.stride = sizeof(PolygenInstance);
		return std::vector{ vibd };
	}

 	static  std::vector<VkVertexInputAttributeDescription> getDescription()
	{
		VkVertexInputAttributeDescription attr;
		std::vector<VkVertexInputAttributeDescription> attrs;
		attrs.reserve(8);
	
		attr.binding = 0;
		attr.location = 0;
		attr.format = VK_FORMAT_R32G32B32A32_SFLOAT;
		attr.offset = offsetof(PolygenInstance,transform);
		attrs.push_back(attr);

		attr.binding = 0;
		attr.location = 1;
		attr.format = VK_FORMAT_R32G32B32A32_SFLOAT;
		attr.offset = offsetof(PolygenInstance, point12);
		attrs.push_back(attr);

		attr.binding = 0;
		attr.location = 2;
		attr.format = VK_FORMAT_R32G32B32A32_SFLOAT;
		attr.offset = offsetof(PolygenInstance, point34);
		attrs.push_back(attr);

		attr.binding =0 ;
		attr.location = 3;
		attr.format = VK_FORMAT_R32G32B32A32_SFLOAT;
		attr.offset = offsetof(PolygenInstance, point56);
		attrs.push_back(attr);

		attr.binding = 0;
		attr.location = 4;
		attr.format = VK_FORMAT_R32G32B32A32_SFLOAT;
		attr.offset = offsetof(PolygenInstance, point78);
		attrs.push_back(attr);

		attr.binding = 0;
		attr.location = 5;
		attr.format = VK_FORMAT_R32G32B32A32_SFLOAT;
		attr.offset = offsetof(PolygenInstance,color);
		attrs.push_back(attr);

		attr.binding = 0;
		attr.location = 6;
		attr.format = VK_FORMAT_R32_SINT;
		attr.offset = offsetof(PolygenInstance, pointsCount);
		attrs.push_back(attr);

		attr.binding = 0;
		attr.location = 7;
		attr.format = VK_FORMAT_R32_SFLOAT;
		attr.offset = offsetof(PolygenInstance, rad);
		attrs.push_back(attr);
		
		return attrs;
	}
};
struct Line {
	Point p1;
	Point p2;
	glm::vec4 color;
	static std::vector<VkVertexInputBindingDescription> getBinding()
	{

		VkVertexInputBindingDescription vibd;
		vibd.binding = 0;
		vibd.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;
		vibd.stride = sizeof(Line);
		return std::vector{ vibd };
	}

	static  std::vector<VkVertexInputAttributeDescription> getDescription()
	{
		VkVertexInputAttributeDescription attr;
		std::vector<VkVertexInputAttributeDescription> attrs;
		attrs.reserve(3);

		attr.binding = 0;
		attr.location = 0;
		attr.format = VK_FORMAT_R32G32_SFLOAT;
		attr.offset = offsetof(Line, p1);
		attrs.push_back(attr);

		attr.binding = 0;
		attr.location = 1;
		attr.format = VK_FORMAT_R32G32_SFLOAT;
		attr.offset = offsetof(Line, p2);
		attrs.push_back(attr);

		attr.binding = 0;
		attr.location = 2;
		attr.format = VK_FORMAT_R32G32B32A32_SFLOAT;
		attr.offset = offsetof(Line, color);
		attrs.push_back(attr);



		return attrs;
	}

};

struct CircleInstance {

	glm::vec4 transform;
	float radius;
	glm::vec4 color;

	static std::vector<VkVertexInputBindingDescription> getBinding()
	{

		VkVertexInputBindingDescription vibd;
		vibd.binding = 0;
		vibd.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;
		vibd.stride = sizeof(CircleInstance);
		return std::vector{ vibd };
	}


	static  std::vector<VkVertexInputAttributeDescription> getDescription()
	{
		VkVertexInputAttributeDescription attr;
		std::vector<VkVertexInputAttributeDescription> attrs;
		attrs.reserve(3);

		attr.binding = 0;
		attr.location = 0;
		attr.format = VK_FORMAT_R32G32B32A32_SFLOAT;
		attr.offset = offsetof(CircleInstance, transform);
		attrs.push_back(attr);

		attr.binding = 0;
		attr.location = 1;
		attr.format = VK_FORMAT_R32_SFLOAT;
		attr.offset = offsetof(CircleInstance, radius);
		attrs.push_back(attr);

		attr.binding = 0;
		attr.location = 2;
		attr.format = VK_FORMAT_R32G32B32A32_SFLOAT;
		attr.offset = offsetof(CircleInstance, color);
		attrs.push_back(attr);

		return attrs;
	}

};
class PolygonRender
{
	int maxPolyCount = 100;
	int maxCircleCount = 100;
	int maxLineCount = 100;
	Pipeline polyPipeline;
	Pipeline pipeline_cir;
	Pipeline pipeline_line;
	VertexBuffer vtx_poly[MAX_FRAMES_IN_FLIGHT];
	VertexBuffer vtx_circle[MAX_FRAMES_IN_FLIGHT];
	VertexBuffer vtx_line[MAX_FRAMES_IN_FLIGHT];

public:
	std::vector<PolygenInstance> polys;
	std::vector<CircleInstance> cirs;
	std::vector<Line> lines;
	
	void init(VkRenderPass pass);
	void setLineWidth(CommandBuffer& cmd,  float w=2);
	void createPipeline(VkRenderPass pass);
	void drawPolygon(const Transform2d& transform, const Point* points, int count, float radius, const glm::vec4& color);
	void drawCircle(const Transform2d& trans, float rad, const glm::vec4& color);
	void drawLine(const Point& pt1, const Point& pt2, const glm::vec4& color);
	void flushPloys(CommandBuffer& cmd,int frame);
	void flushCircles(CommandBuffer& cmd, int frame);
	void flushLines(CommandBuffer& cmd, int frame);

	void clean();
};

