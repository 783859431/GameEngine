#pragma once
#include <vector>
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <array>
#include "../VertexBuffer.h"
#include "../IndexBuffer.h"
#include "../CommandBuffer.h"

struct Vertex
{
    glm::vec3 pos;
    glm::vec2 uv;
    union 
    {
      glm::vec3 color;
      glm::vec3 normal;
    };

    static std::array<VkVertexInputBindingDescription, 1> getBindingDescription()
    {
        std::array<VkVertexInputBindingDescription, 1> bindingDescriptions{};
        bindingDescriptions[0].binding = 0;
        bindingDescriptions[0].stride = sizeof(Vertex);
        bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescriptions;
    }

    static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions()
    {
        std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, uv);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(Vertex, color);
        return attributeDescriptions;
    }
};

class Mesh
{
public:
    VertexBuffer vb;
    IndexBuffer ib;
    
    void create(std::vector<Vertex>& vts,std::vector<uint32_t>& idx);
    void create(std::vector<Vertex>& vts);
    void drawIndexed(CommandBuffer& cmd);
    void draw(CommandBuffer& cmd);
    void createQuad(float width, float height);
    void createSphere(float rad, int sectors, int stacks);
    void createCircle(float rad,float x =0.0f,float y =0.0f, int segments = 64);
    void createCube(float size);
    void createFromFile(const std::string& path);

    VkBuffer getIndexBuffer()
    {
        return ib.getBuffer();
    }
    VkBuffer getVertexBuffer() {
        return vb.getBuffer();
    }
    // 生成几何体数据
    static void generateQuad(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices,
        float width, float height, bool colored);
    static void generateCube(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices,
        float size, bool colored);
    static void generateSphere(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices,
        float radius, int sectors, int stacks, bool colored);
    static void generateCircle(std::vector<Vertex>& verts,std::vector<uint32_t>& ids, float rad, float cx =0.0, float cy =0.0, int segments =64);
};

