#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
class Transform2D
{
public:
    Transform2D() = default;
    explicit Transform2D(const glm::vec2& position) : m_position(position) {}
    Transform2D(const glm::vec2& position, float rotation, const glm::vec2& scale = glm::vec2(1.0f))
        : m_position(position), m_rotation(rotation), m_scale(scale) {}

    // --- 本地变换属性 ---

    /** 获取本地位置 */
    glm::vec2 getLocalPosition() const { return m_position; }
    /** 设置本地位置 */
    void setLocalPosition(const glm::vec2& position) {
        m_position = position;
        m_dirty = true;
    }

    /** 获取本地旋转（弧度） */
    float getLocalRotation() const { return m_rotation; }
    /** 设置本地旋转（弧度） */
    void setLocalRotation(float rotation) {
        m_rotation = rotation;
        m_dirty = true;
    }

    /** 获取本地缩放 */
    glm::vec2 getLocalScale() const { return m_scale; }
    /** 设置本地缩放 */
    void setLocalScale(const glm::vec2& scale) {
        m_scale = scale;
        m_dirty = true;
    }

    /** 获取本地变换矩阵（4x4，相对于父节点） */
    glm::mat4 getLocalMatrix() {
        if (m_dirty) {
            recalculateLocalMatrix();
        }
        return m_localMatrix;
    }

    // --- 世界变换属性（考虑父节点） ---

    /** 获取世界位置（如果无父节点则等于本地位置） */
    glm::vec2 getWorldPosition() {
        auto worldMat = getWorldMatrix();
        return glm::vec2(worldMat[3][0], worldMat[3][1]);
    }

    /** 获取世界旋转（弧度），累积所有父节点的旋转 */
    float getWorldRotation() {
        float total = m_rotation;
        Transform2D* parent = m_parent;
        while (parent) {
            total += parent->m_rotation;
            parent = parent->m_parent;
        }
        return total;
    }

    /** 获取世界缩放（累积缩放） */
    glm::vec2 getWorldScale() {
        glm::vec2 total = m_scale;
        Transform2D* parent = m_parent;
        while (parent) {
            total *= parent->m_scale;
            parent = parent->m_parent;
        }
        return total;
    }

    /** 获取世界变换矩阵（4x4，适用于 Vulkan） */
    glm::mat4 getWorldMatrix() {
        if (m_parent) {
            // 世界矩阵 = 父世界矩阵 * 本地矩阵
            return m_parent->getWorldMatrix() * getLocalMatrix();
        }
        return getLocalMatrix();
    }

    // --- 层级管理 ---

    /** 设置父节点（自动维护父子关系） */
    void setParent(Transform2D* parent) {
        // 如果已有父节点，先从旧父节点中移除自己
        if (m_parent) {
            auto& siblings = m_parent->m_children;
            auto it = std::find(siblings.begin(), siblings.end(), this);
            if (it != siblings.end()) {
                siblings.erase(it);
            }
        }

        m_parent = parent;
        if (parent) {
            parent->m_children.push_back(this);
        }
        m_dirty = true;
    }

    /** 获取父节点指针 */
    Transform2D* getParent() const { return m_parent; }

    /** 获取子节点列表（只读） */
    const std::vector<Transform2D*>& getChildren() const { return m_children; }

    // --- 便捷移动/旋转/缩放方法 ---

    /** 相对当前位置平移 */
    void translate(const glm::vec2& offset) {
        m_position += offset;
        m_dirty = true;
    }

    /** 相对当前旋转增加弧度 */
    void rotate(float deltaRadians) {
        m_rotation += deltaRadians;
        m_dirty = true;
    }

    /** 相对当前缩放放大 */
    void scaleBy(const glm::vec2& factor) {
        m_scale *= factor;
        m_dirty = true;
    }

    // --- 坐标转换工具 ---

    /** 将本地坐标转换为世界坐标 */
    glm::vec2 localToWorld(const glm::vec2& localPoint) {
        auto mat = getWorldMatrix();
        glm::vec4 p(localPoint, 0.0f, 1.0f);
        p = mat * p;
        return glm::vec2(p.x, p.y);
    }

    /** 将世界坐标转换为本地坐标 */
    glm::vec2 worldToLocal(const glm::vec2& worldPoint) {
        auto mat = getWorldMatrix();
        glm::vec4 p(worldPoint, 0.0f, 1.0f);
        p = glm::inverse(mat) * p;
        return glm::vec2(p.x, p.y);
    }

    /** 获取直接可用于 Vulkan 的矩阵指针（列主序） */
    const float* getMatrixPtr() {
        return glm::value_ptr(getWorldMatrix());
    }

private:
    /** 重新计算本地矩阵（当属性变脏时调用） */
    void recalculateLocalMatrix() {
  
        glm::mat4 trans = glm::translate(glm::mat4(1.0f), glm::vec3(m_position, 0.0f));
        glm::mat4 rot = glm::rotate(glm::mat4(1.0f), m_rotation, glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 sca = glm::scale(glm::mat4(1.0f), glm::vec3(m_scale, 1.0f));
        m_localMatrix = trans * rot * sca;
        m_dirty = false;

      
        for (auto* child : m_children) {
            child->m_dirty = true;
        }
    }

    glm::vec2 m_position = glm::vec2(0.0f);
    float m_rotation = 0.0f;                // 弧度
    glm::vec2 m_scale = glm::vec2(1.0f);    // V

    glm::mat4 m_localMatrix = glm::mat4(1.0f);
    bool m_dirty = true;

    Transform2D* m_parent = nullptr;
    std::vector<Transform2D*> m_children;


};

class Transform3D
{


};
