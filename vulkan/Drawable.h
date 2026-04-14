#pragma once
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "CommandBuffer.h"
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

struct Mesh
{
    VertexBuffer vertexBuffer;
    IndexBuffer indexBuffer;
};
class Material
{
public:
    VkPipeline pipeline;
    VkPipelineLayout layout;

    virtual void bind(VkCommandBuffer cmd) = 0;
};


struct DrawContext
{
    CommandBuffer cmd;
    uint32_t frameIndex;
};

class Drawable
{
public:
	virtual ~Drawable() = default;
	virtual void record(DrawContext& ctx) = 0;


};


struct Transform 
{
    glm::vec3 position;
    glm::quat rotation{ 1, 0, 0, 0 };
    glm::vec3 scale;

    void RotateEuler(const glm::vec3& eulerRad)
    {
        glm::quat q = glm::quat(eulerRad);
        rotation = glm::normalize(q * rotation);
    }

    void RotateAxis(float angleRad, const glm::vec3& axis)
    {
        glm::quat q = glm::angleAxis(angleRad, axis);
        rotation = glm::normalize(q * rotation);
    }

    glm::mat4 GetModelMatrix() const
    {
        return glm::translate(glm::mat4(1.0f), position)
              *glm::toMat4(rotation)
              *glm::scale(glm::mat4(1.0f), scale);
    }
    glm::vec3 GetEulerAngs() const 
    {
        return  glm::eulerAngles(rotation);
    }

};


class DrawableMesh : public Drawable
{
public:
    Mesh* mesh;
    Material* material;
    Transform transform;
    VkDescriptorSet perObjectSet; // transform / color

    void record(DrawContext& ctx) override
    {
        material->bind(ctx.cmd.command);
        vkCmdBindDescriptorSets(
            ctx.cmd.command,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            material->layout,
            0,
            1,
            &perObjectSet,
            0,
            nullptr
        );

        VkDeviceSize offset = 0;
        vkCmdBindVertexBuffers(ctx.cmd.command, 0, 1, &mesh->vertexBuffer.GetBuffer(), &offset);
        vkCmdBindIndexBuffer(ctx.cmd.command, mesh->indexBuffer.GetBuffer(), 0, VK_INDEX_TYPE_UINT32);
        vkCmdDrawIndexed(ctx.cmd.command, mesh->indexBuffer.indexCount, 1, 0, 0, 0);
    }
};

