#pragma once
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_LEFT_HANDED
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../UniformBuffer.h"
enum class ProjectionType {
    PERSPECTIVE,
    ORTHOGRAPHIC
};
struct CamData
{
    glm::mat4 v;
    glm::mat4 p;
};
class Camera {
public:
    // 构造函数
    Camera();

    // 设置投影类型
    void setProjectionType(ProjectionType type);

    // 透视投影参数设置 (fov 为角度制)
    void setPerspective(float fov, float aspect, float nearPlane, float farPlane);

    // 正交投影参数设置
    void setOrthographic(float left, float right, float bottom, float top, float nearPlane, float farPlane);

    // 移动摄像机到世界空间的任意位置
    void setPosition(const glm::vec3& position);

    // 设置摄像机看向的目标点
    void lookAt(const glm::vec3& target);

    // 设置摄像机的上方向
    void setUpVector(const glm::vec3& up);

    // 平移摄像机（相对移动）
    void translate(const glm::vec3& delta);

    // 获取视图矩阵
    glm::mat4 getViewMatrix() const;

    // 获取投影矩阵
    glm::mat4 getProjectionMatrix() const;

    // 获取视图投影矩阵
    glm::mat4 getViewProjectionMatrix() const;

    // 获取摄像机位置
    glm::vec3 getPosition() const { return m_position; }

    // 获取摄像机朝向（前方向量）
    glm::vec3 getForward() const { return m_forward; }

    // 获取摄像机的右方向量
    glm::vec3 getRight() const { return m_right; }

    // 获取摄像机的上方向量
    glm::vec3 getUp() const { return m_up; }

    // 更新摄像机内部向量（在改变位置、目标点或上方向后调用）
    void update();
    void updateData(UniformBuffer& ub);
    // 摄像机在世界空间中的位置
    glm::vec3 m_position;
private:


    // 摄像机看向的目标点
    glm::vec3 m_target;

    // 摄像机的上方向
    glm::vec3 m_up;

    // 摄像机的三个基向量（相机坐标系）
    glm::vec3 m_forward;  // 前向（视线方向）
    glm::vec3 m_right;    // 右向
    glm::vec3 m_cameraUp; // 上向（垂直于前向和右向）

    // 投影参数
    ProjectionType m_projectionType;

    // 透视投影参数
    float m_fov;
    float m_aspect;
    float m_nearPlane;
    float m_farPlane;

    // 正交投影参数
    float m_orthoLeft;
    float m_orthoRight;
    float m_orthoBottom;
    float m_orthoTop;

    // 标记是否需要重新计算矩阵
    mutable bool m_viewDirty;
    mutable bool m_projDirty;

    // 缓存的矩阵
    mutable glm::mat4 m_viewMatrix;
    mutable glm::mat4 m_projectionMatrix;
    // 

};
void WorldToScreen(const Camera& cam, const glm::vec3& world, glm::vec2& screen);
void ScreenToWorld(const Camera& cam, const glm::vec2& screen, glm::vec3& world, float ndcZ = 0.0f);