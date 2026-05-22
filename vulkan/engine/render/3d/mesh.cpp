#include "mesh.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include "../../engine/utils/FileUtils.h"
#include "../../logger/Logger.h"
void Mesh::create(std::vector<Vertex>& vts, std::vector<uint32_t>& idx)
{
    if (vts.size() < 3)
        return;
    int size = vts.size() * sizeof(Vertex);
    vb.allocBuffer(size);
    vb.vertCount = vts.size();
    vb.updateVextex(vts.data(), size);


    if (idx.size() == 0)
        return;
    size = idx.size() * sizeof(int);
    ib.allocBuffer(size);
    ib.updateIndex(idx.data(),size);


}

void Mesh::create(std::vector<Vertex>& vts)
{
    if (vts.size() < 3)
        return;
    int size = vts.size() * sizeof(Vertex);
    vb.allocBuffer(size);
    vb.vertCount = vts.size();
    vb.updateVextex(vts.data(), size);

}

void Mesh::drawIndexed(CommandBuffer& cmd)
{
    cmd.bindVertex(getVertexBuffer());
    cmd.bindIndex(getIndexBuffer());
    cmd.drawIndexed(ib.indexCount);
}
void Mesh::draw(CommandBuffer& cmd)
{
    cmd.bindVertex(getVertexBuffer());
    cmd.draw(vb.vertCount);
}
void Mesh::createQuad(float width,float height)
{
    std::vector<Vertex> vts;
    std::vector<uint32_t> ids;
    Mesh::generateQuad(vts,ids,width,height,false);
    create(vts, ids);
}

void Mesh::createSphere(float rad, int sectors, int stacks)
{
    std::vector<Vertex> vts;
    std::vector<uint32_t> ids;
    Mesh::generateSphere(vts, ids,rad, sectors, stacks,false);
    create(vts, ids);

}

void Mesh::createCircle(float rad, float x,float y, int segments)
{
    std::vector<Vertex> vts;
    std::vector<uint32_t> ids;
    Mesh::generateCircle(vts, ids, rad, x, y, segments);
    create(vts, ids);
}

void Mesh::createCube(float size)
{

    std::vector<Vertex> vts;
    std::vector<uint32_t> ids;
    Mesh::generateCube(vts, ids, size, false);
    create(vts, ids);
}

void Mesh::createFromFile(const std::string& path)
{
    std::vector<Vertex> outVertices;
    std::vector<uint32_t> outIndices;
    std::string full = FileUtils::GetInst()->GetFileFullPath(path.c_str());
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string warn;
    std::string err;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, full.c_str());

    if (!warn.empty()) {
        std::cout << warn << std::endl;
        Logger::getInstance().warn(warn.c_str());
    }

    if (!err.empty()) {
        Logger::getInstance().error(err.c_str());
    }
    // 遍历所有形状和所有面片索引
    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            Vertex vertex{};

            // 从 attrib 中依据索引提取数据
            vertex.pos = {
                attrib.vertices[3 * index.vertex_index + 0],
                attrib.vertices[3 * index.vertex_index + 1],
                attrib.vertices[3 * index.vertex_index + 2]
            };

            // 提取法线 (如果存在)
            if (!attrib.normals.empty()) {
                vertex.normal = {
                    attrib.normals[3 * index.normal_index + 0],
                    attrib.normals[3 * index.normal_index + 1],
                    attrib.normals[3 * index.normal_index + 2]
                };
            }

            // 提取UV坐标 (如果存在)
            if (!attrib.texcoords.empty()) {
                vertex.uv = {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    1.0f - attrib.texcoords[2 * index.texcoord_index + 1] // Vulkan的UV原点在左上角
                };
            }

            outVertices.push_back(vertex);
            outIndices.push_back(outIndices.size());  // 索引就是顺序的
        }
    }

    create(outVertices, outIndices);
}





void Mesh::generateQuad(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices,
    float width, float height, bool colored) {
    float halfWidth = width * 0.5f;
    float halfHeight = height * 0.5f;

    vertices = {
        {{-halfWidth, -halfHeight, 0.0f}, {0.0f, 0.0f},glm::vec3(0.0f, 0.0f, 1.0f)},
        {{ halfWidth, -halfHeight, 0.0f} ,{1.0f, 0.0f},glm::vec3(0.0f, 0.0f, 1.0f)},
        {{ halfWidth,  halfHeight, 0.0f}, {1.0f, 1.0f},glm::vec3(0.0f, 0.0f, 1.0f)},
        {{-halfWidth,  halfHeight, 0.0f}, {0.0f, 1.0f},glm::vec3(0.0f, 0.0f, 1.0f)}
    };

    indices = { 0, 1, 2, 2, 3, 0 };
}

void Mesh::generateCube(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices,
    float size, bool colored) {
    float half = size * 0.5f;

    // 6个面，每个面4个顶点
    vertices = {
        // 前面 (Z+)
        {{-half, -half,  half},{0.0f, 0.0f}, glm::vec3(0.0f, 0.0f, 1.0f)},
        {{ half, -half,  half},{1.0f, 0.0f}, glm::vec3(0.0f, 0.0f, 1.0f)},
        {{ half,  half,  half},{1.0f, 1.0f}, glm::vec3(0.0f, 0.0f, 1.0f)},
        {{-half,  half,  half},{0.0f, 1.0f}, glm::vec3(0.0f, 0.0f, 1.0f)},

        // 后面 (Z-)
        {{-half, -half, -half},{0.0f, 0.0f}, glm::vec3(0.0f, 0.0f, -1.0f)},
        {{ half, -half, -half},{1.0f, 0.0f}, glm::vec3(0.0f, 0.0f, -1.0f)},
        {{ half,  half, -half},{1.0f, 1.0f}, glm::vec3(0.0f, 0.0f, -1.0f)},
        {{-half,  half, -half},{0.0f, 1.0f}, glm::vec3(0.0f, 0.0f, -1.0f)},

        // 左面 (X-)
        {{-half, -half, -half},{0.0f, 0.0f}, glm::vec3(-1.0f, 0.0f,0.0f)},
        {{-half, -half,  half},{1.0f, 0.0f}, glm::vec3(-1.0f, 0.0f,0.0f)},
        {{-half,  half,  half},{1.0f, 1.0f}, glm::vec3(-1.0f, 0.0f,0.0f)},
        {{-half,  half, -half},{0.0f, 1.0f}, glm::vec3(-1.0f, 0.0f,0.0f)},

        // 右面 (X+)
        {{ half, -half, -half},{0.0f, 0.0f}, glm::vec3(1.0f, 0.0f, 0.0f)},
        {{ half, -half,  half},{1.0f, 0.0f}, glm::vec3(1.0f, 0.0f, 0.0f)},
        {{ half,  half,  half},{1.0f, 1.0f}, glm::vec3(1.0f, 0.0f, 0.0f)},
        {{ half,  half, -half},{0.0f, 1.0f}, glm::vec3(1.0f, 0.0f, 0.0f)},

        // 下面 (Y-)
        {{-half, -half, -half},{0.0f, 0.0f}, glm::vec3(0.0f, 1.0f,0.0f)},
        {{ half, -half, -half},{1.0f, 0.0f}, glm::vec3(0.0f, 1.0f,0.0f)},
        {{ half, -half,  half},{1.0f, 1.0f}, glm::vec3(0.0f, 1.0f,0.0f)},
        {{-half, -half,  half},{0.0f, 1.0f}, glm::vec3(0.0f, 1.0f,0.0f)},
                                             
        // 上面 (Y+)
        {{-half,  half, -half},{0.0f, 0.0f}, glm::vec3(0.0f, -1.0f, 0.0f)},
        {{ half,  half, -half},{1.0f, 0.0f}, glm::vec3(0.0f, -1.0f, 0.0f)},
        {{ half,  half,  half},{1.0f, 1.0f}, glm::vec3(0.0f, -1.0f, 0.0f)},
        {{-half,  half,  half},{0.0f, 1.0f}, glm::vec3(0.0f, -1.0f, 0.0f)}
    };

    // 索引：每个面两个三角形
    indices = {

           0,  3,  2,   2,  1,  0, 
           4,  5,  6,   6,  7,  4, 
           8,  11, 10,  10, 9,  8, 
           12, 14, 15,  13, 14, 12, 
           16, 19, 18,  18, 17, 16,  
           20, 21, 22,  22, 23, 20  
    };
}

void Mesh::generateSphere(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices,
    float radius, int sectors, int stacks, bool colored) {
    vertices.clear();
    indices.clear();

    for (int i = 0; i <= stacks; ++i) {
        float v = static_cast<float>(i) / static_cast<float>(stacks);
        float phi = v * 3.14159f;

        for (int j = 0; j <= sectors; ++j) {
            float u = static_cast<float>(j) / static_cast<float>(sectors);
            float theta = u * 2.0f * 3.14159f;

            float x = radius * sin(phi) * cos(theta);
            float y = radius * cos(phi);
            float z = radius * sin(phi) * sin(theta);

            glm::vec3 normal = glm::normalize(glm::vec3(x, y, z));
            glm::vec2 texCoord = glm::vec2(u, v);
            glm::vec3 color =  glm::vec3(1.0f);//colored ? glm::vec3(u, v, 1.0f - u) :

            vertices.push_back({ glm::vec3(x, y, z),texCoord,normal });
        }
    }

    for (int i = 0; i < stacks; ++i) {
        for (int j = 0; j < sectors; ++j) {
            int first = i * (sectors + 1) + j;
            int second = first + sectors + 1;

            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);

            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }
}

void Mesh::generateCircle(std::vector<Vertex>& verts,std::vector<uint32_t>& ids, float rad, float cx, float cy, int segments)
{

    const float PI = 3.1415926f;
    verts.push_back({ {cx,cy,0.0f},{0.0f,0.0f},glm::vec3{0.0f,0.0f,1.0f} });

    for (int i = 0; i < segments; i++)
    {
        float angle = (float)i / segments * 2.0f * PI;

        float x = cx+cos(angle) * rad;
        float y = cy+sin(angle) * rad;

        verts.push_back({ {x,y,0.0f},{0.0f,0.0f},glm::vec3{0.0f,0.0f,1.0f} });
        ids.push_back(0);
        ids.push_back(i+1);
        int id = i + 2;
        if (id >= segments + 1)
            id = 1;
        ids.push_back(id);
    }
    
    //012 023 034 045

}
