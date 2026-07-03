#include "Camera.h"
CamData camData;
Camera::Camera()
    : m_position(0.0f, 0.0f, -5.0f)
    , m_target(0.0f, 0.0f, 0.0f)
    , m_up(0.0f, 1.0f, 0.0f)
    , m_projectionType(ProjectionType::ORTHOGRAPHIC)
    , m_fov(45.0f)
    , m_aspect(16.0f / 9.0f)
    , m_nearPlane(0.1f)
    , m_farPlane(100.0f)
    , m_orthoLeft(-10.0f)
    , m_orthoRight(10.0f)
    , m_orthoBottom(-10.0f)
    , m_orthoTop(10.0f)
    , m_viewDirty(true)
    , m_projDirty(true)
{
    update();
}

void Camera::setProjectionType(ProjectionType type) {
    m_projectionType = type;
    m_projDirty = true;
}

void Camera::setPerspective(float fov, float aspect, float nearPlane, float farPlane) {
    m_fov = fov;
    m_aspect = aspect;
    m_nearPlane = nearPlane;
    m_farPlane = farPlane;
    m_projectionType = ProjectionType::PERSPECTIVE;
    m_projDirty = true;
}

void Camera::setOrthographic(float left, float right, float bottom, float top, float nearPlane, float farPlane) {
    m_orthoLeft = left;
    m_orthoRight = right;
    m_orthoBottom = bottom;
    m_orthoTop = top;
    m_nearPlane = nearPlane;
    m_farPlane = farPlane;
    m_projectionType = ProjectionType::ORTHOGRAPHIC;
    m_projDirty = true;
}

void Camera::setPosition(const glm::vec3& position) {
    m_position = position;
    m_viewDirty = true;
}

void Camera::lookAt(const glm::vec3& target) {
    m_target = target;
    m_viewDirty = true;
}

void Camera::setUpVector(const glm::vec3& up) {
    m_up = up;
    m_viewDirty = true;
}

void Camera::translate(const glm::vec3& delta) {
    m_position += delta;
    m_target += delta;  // 保持视线方向不变，平移整个相机和目标
    m_viewDirty = true;
}

void Camera::update() 
{
    // 计算前方向量（从相机位置指向目标点）
    m_forward = glm::normalize(m_target - m_position);

    // 计算右方向量（前向和世界上的叉积）
    m_right = glm::normalize(glm::cross(m_forward, m_up));

    // 计算相机的真实上方向（垂直于前向和右向）
    m_cameraUp = glm::normalize(glm::cross(m_right, m_forward));
    m_viewDirty = true;

}

void Camera::updateData(UniformBuffer& ub)
{
    camData.p = getProjectionMatrix();
    camData.v = getViewMatrix();
    ub.updateData(&camData,sizeof(camData));

}

glm::mat4 Camera::getViewMatrix() const {
    if (m_viewDirty) {
        // 使用 glm::lookAt 生成视图矩阵
        m_viewMatrix = glm::lookAt(m_position, m_target, m_up);
        m_viewDirty = false;
    }
    return m_viewMatrix;
}

glm::mat4 Camera::getProjectionMatrix() const {
    if (m_projDirty) {
        if (m_projectionType == ProjectionType::PERSPECTIVE) {
            m_projectionMatrix = glm::perspective(
                glm::radians(m_fov),
                m_aspect,
                m_nearPlane,
                m_farPlane
            );
        }
        else {
            m_projectionMatrix = glm::ortho(
                m_orthoLeft, m_orthoRight,
                m_orthoBottom, m_orthoTop,
                m_nearPlane, m_farPlane
            );
        }
        m_projDirty = false;
    }
    return m_projectionMatrix;
}

glm::mat4 Camera::getViewProjectionMatrix() const {
    return getProjectionMatrix() * getViewMatrix();
}