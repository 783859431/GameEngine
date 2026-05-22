#include "transform.h"
#include <cmath>

// 构造函数实现
TransformComponent::TransformComponent()
    : m_position(0.0f, 0.0f, 0.0f)
    , m_rotationEuler(0.0f, 0.0f, 0.0f)
    , m_rotationQuat(1.0f, 0.0f, 0.0f, 0.0f)
    , m_scale(1.0f, 1.0f, 1.0f)
    , m_dirty(true) {}

TransformComponent::TransformComponent(const glm::vec3& position)
    : m_position(position)
    , m_rotationEuler(0.0f, 0.0f, 0.0f)
    , m_rotationQuat(1.0f, 0.0f, 0.0f, 0.0f)
    , m_scale(1.0f, 1.0f, 1.0f)
    , m_dirty(true) {}

TransformComponent::TransformComponent(const glm::vec3& position, const glm::vec3& rotation)
    : m_position(position)
    , m_rotationEuler(rotation)
    , m_scale(1.0f, 1.0f, 1.0f)
    , m_dirty(true) {
    updateRotationQuat();
}

TransformComponent::TransformComponent(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale)
    : m_position(position)
    , m_rotationEuler(rotation)
    , m_scale(scale)
    , m_dirty(true) {
    updateRotationQuat();
}

// 位置设置
void TransformComponent::setPosition(const glm::vec3& position) {
    if (m_position != position) {
        m_position = position;
        m_dirty = true;
    }
}

void TransformComponent::setPosition(float x, float y, float z) {
    setPosition(glm::vec3(x, y, z));
}

void TransformComponent::translate(const glm::vec3& delta) {
    if (delta != glm::vec3(0.0f)) {
        m_position += delta;
        m_dirty = true;
    }
}

void TransformComponent::translate(float x, float y, float z) {
    translate(glm::vec3(x, y, z));
}

// 旋转设置（欧拉角）
void TransformComponent::setRotation(const glm::vec3& rotation) {
    if (m_rotationEuler != rotation) {
        m_rotationEuler = rotation;
        updateRotationQuat();
        m_dirty = true;
    }
}

void TransformComponent::setRotation(float pitch, float yaw, float roll) {
    setRotation(glm::vec3(pitch, yaw, roll));
}

void TransformComponent::rotate(const glm::vec3& delta) {
    if (delta != glm::vec3(0.0f)) {
        m_rotationEuler += delta;
        updateRotationQuat();
        m_dirty = true;
    }
}

void TransformComponent::rotate(float pitch, float yaw, float roll) {
    rotate(glm::vec3(pitch, yaw, roll));
}

// 旋转设置（四元数）
void TransformComponent::setRotationQuat(const glm::quat& quaternion) {
    if (m_rotationQuat != quaternion) {
        m_rotationQuat = quaternion;
        // 从四元数反推欧拉角（可选，用于保持一致性）
        // m_rotationEuler = glm::eulerAngles(quaternion);
        m_dirty = true;
    }
}

void TransformComponent::rotateQuat(const glm::quat& delta) {
    m_rotationQuat = delta * m_rotationQuat;
    m_rotationQuat = glm::normalize(m_rotationQuat);
    // m_rotationEuler = glm::eulerAngles(m_rotationQuat);
    m_dirty = true;
}

// 缩放设置
void TransformComponent::setScale(const glm::vec3& scale) {
    if (m_scale != scale) {
        // 防止零或负缩放
        m_scale = glm::max(scale, glm::vec3(0.0001f));
        m_dirty = true;
    }
}

void TransformComponent::setScale(float uniformScale) {
    setScale(glm::vec3(uniformScale));
}

void TransformComponent::setScale(float x, float y, float z) {
    setScale(glm::vec3(x, y, z));
}

void TransformComponent::scaleBy(const glm::vec3& multiplier) {
    setScale(m_scale * multiplier);
}

// 获取模型矩阵
const glm::mat4& TransformComponent::getModelMatrix() {
    if (m_dirty) {
        updateModelMatrix();
    }
    return m_modelMatrix;
}

// 获取逆模型矩阵
const glm::mat4& TransformComponent::getInverseModelMatrix() {
    if (m_dirty) {
        updateModelMatrix();
    }
    return m_inverseModelMatrix;
}

// 重置变换
void TransformComponent::reset() {
    m_position = glm::vec3(0.0f);
    m_rotationEuler = glm::vec3(0.0f);
    m_rotationQuat = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    m_scale = glm::vec3(1.0f);
    m_dirty = true;
}

// 获取方向向量
glm::vec3 TransformComponent::getForward() const {
    // 前向是Z轴正方向
    return glm::normalize(m_rotationQuat * glm::vec3(0.0f, 0.0f, 1.0f));
}

glm::vec3 TransformComponent::getRight() const {
    // 右向是X轴正方向
    return glm::normalize(m_rotationQuat * glm::vec3(1.0f, 0.0f, 0.0f));
}

glm::vec3 TransformComponent::getUp() const {
    // 上向是Y轴正方向
    return glm::normalize(m_rotationQuat * glm::vec3(0.0f, 1.0f, 0.0f));
}

// 快捷设置函数
void TransformComponent::setTransform(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale) {
    m_position = position;
    m_rotationEuler = rotation;
    m_scale = glm::max(scale, glm::vec3(0.0001f));
    updateRotationQuat();
    m_dirty = true;
}

// 变换点
glm::vec3 TransformComponent::transformPoint(const glm::vec3& point) const {
    // 先缩放，再旋转，再平移
    return m_position + (m_rotationQuat * (point * m_scale));
}

glm::vec3 TransformComponent::transformDirection(const glm::vec3& direction) const {
    // 只应用旋转，不应用平移和缩放
    return glm::normalize(m_rotationQuat * direction);
}

// 操作符重载
TransformComponent& TransformComponent::operator=(const TransformComponent& other) {
    if (this != &other) {
        m_position = other.m_position;
        m_rotationEuler = other.m_rotationEuler;
        m_rotationQuat = other.m_rotationQuat;
        m_scale = other.m_scale;
        m_dirty = true;
    }
    return *this;
}

bool TransformComponent::operator==(const TransformComponent& other) const {
    return m_position == other.m_position &&
        m_rotationEuler == other.m_rotationEuler &&
        m_scale == other.m_scale;
}

bool TransformComponent::operator!=(const TransformComponent& other) const {
    return !(*this == other);
}

// 私有辅助函数
void TransformComponent::updateRotationQuat() {
    // 从欧拉角创建四元数（按ZYX顺序，即roll, yaw, pitch）
    glm::quat pitchQuat = glm::angleAxis(m_rotationEuler.x, glm::vec3(1.0f, 0.0f, 0.0f));
    glm::quat yawQuat = glm::angleAxis(m_rotationEuler.y, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::quat rollQuat = glm::angleAxis(m_rotationEuler.z, glm::vec3(0.0f, 0.0f, 1.0f));

    m_rotationQuat = rollQuat * yawQuat * pitchQuat;
    m_rotationQuat = glm::normalize(m_rotationQuat);
}

void TransformComponent::updateModelMatrix() {
    // 构建模型矩阵：缩放 → 旋转 → 平移
    glm::mat4 matrix = glm::mat4(1.0f);
    matrix = glm::translate(matrix, m_position);
    matrix = matrix * glm::toMat4(m_rotationQuat);
    matrix = glm::scale(matrix, m_scale);

    m_modelMatrix = matrix;
    m_inverseModelMatrix = glm::inverse(m_modelMatrix);
    m_dirty = false;
}