#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

class TransformComponent {
public:
    // 构造函数
    TransformComponent();
    TransformComponent(const glm::vec3& position);
    TransformComponent(const glm::vec3& position, const glm::vec3& rotation);
    TransformComponent(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale);

    // 设置位置
    void setPosition(const glm::vec3& position);
    void setPosition(float x, float y, float z);
    void translate(const glm::vec3& delta);
    void translate(float x, float y, float z);

    // 设置旋转（欧拉角，单位：弧度）
    void setRotation(const glm::vec3& rotation);  // 欧拉角 (pitch, yaw, roll)
    void setRotation(float pitch, float yaw, float roll);
    void rotate(const glm::vec3& delta);
    void rotate(float pitch, float yaw, float roll);

    // 设置旋转（四元数）
    void setRotationQuat(const glm::quat& quaternion);
    void rotateQuat(const glm::quat& delta);

    // 设置缩放
    void setScale(const glm::vec3& scale);
    void setScale(float uniformScale);
    void setScale(float x, float y, float z);
    void scaleBy(const glm::vec3& multiplier);

    // 获取变换分量
    const glm::vec3& getPosition() const { return m_position; }
    const glm::vec3& getRotationEuler() const { return m_rotationEuler; }
    const glm::quat& getRotationQuat() const { return m_rotationQuat; }
    const glm::vec3& getScale() const { return m_scale; }

    // 获取模型矩阵（会根据dirty标志自动重新计算）
    const glm::mat4& getModelMatrix();

    // 强制标记为需要更新
    void markDirty() { m_dirty = true; }

    // 检查是否有变化
    bool isDirty() const { return m_dirty; }

    // 重置变换
    void reset();

    // 获取各个方向向量（世界坐标系）
    glm::vec3 getForward() const;   // 前向（Z正方向）
    glm::vec3 getRight() const;     // 右向（X正方向）
    glm::vec3 getUp() const;        // 上向（Y正方向）

    // 获取逆模型矩阵（用于法线变换等）
    const glm::mat4& getInverseModelMatrix();

    // 快捷设置函数：一次性设置所有变换
    void setTransform(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale);

    // 计算变换后的点
    glm::vec3 transformPoint(const glm::vec3& point) const;
    glm::vec3 transformDirection(const glm::vec3& direction) const;

    // 操作符重载
    TransformComponent& operator=(const TransformComponent& other);
    bool operator==(const TransformComponent& other) const;
    bool operator!=(const TransformComponent& other) const;

private:
    glm::vec3 m_position;      // 位置
    glm::vec3 m_rotationEuler; // 旋转（欧拉角，弧度制）
    glm::quat m_rotationQuat;   // 旋转（四元数，缓存）
    glm::vec3 m_scale;         // 缩放

    mutable glm::mat4 m_modelMatrix;      // 缓存的模型矩阵
    mutable glm::mat4 m_inverseModelMatrix; // 缓存的逆模型矩阵
    mutable bool m_dirty;                  // 是否需要重新计算

    // 内部更新函数
    void updateRotationQuat();  // 从欧拉角更新四元数
    void updateModelMatrix();    // 重新计算模型矩阵
};