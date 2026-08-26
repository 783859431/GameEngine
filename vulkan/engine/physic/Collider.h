#pragma once
#include "glm/glm.hpp"
#include "box2d/box2d.h"
#include <functional>
enum ColliderType
{
    NONE,
    BOX,
    BALL
};
class Collider
{
    void createBody(bool isDynamic, const glm::vec2& position, float angle);
public:
    ColliderType type = ColliderType::NONE;

    Collider& operator=(Collider&) = delete;
    Collider(Collider& collider) = delete;

    Collider(b2WorldId world, const glm::vec2& position, float angle, bool isDynamic, bool isBullet, bool allowRot);

    virtual ~Collider();
    // ---------- 物理属性设置 ----------
    void setTransform(const glm::vec2& position, float angle = 0.0f);
    void setDensity(float density);                    //密度
    void setFriction(float friction);                  //摩擦力
    void setRestitution(float restitution);            //弹性系数
    void setGravityScale(float gs);                    //重力系数
    void setEnabled(bool enabled);                     //启用/禁用碰撞响应
    void setLinearVelocity(const glm::vec2& v);        //线速度
    void setAngularVelocity(float rad);                //角速度
    void setLinearDamping(float x);                    //线速度阻尼
    void setAngularDamping(float x);                   //角速度阻尼            
    void setColliderFilter(uint32_t category, uint32_t mask, int groupIndex = 0);
    void applyForce(const glm::vec2& force);           //中心施加力
    void applyForce(const glm::vec2& force, const glm::vec2& point); //在特定点施加力
    void applyTorque(float torque);                    //使用力矩



    void enableSensorEvent(bool);
    void enableHitEvent(bool);
    void enablePreSolveEvent(bool);
    void enableContactEvent(bool);

    // ---------- 获取数据 ----------
    glm::vec2 getPosition() const;
    float getAngle() const;
    glm::vec2 getRot() const;
    bool isSensor() const;
    bool isEnabled() const;
    int getContactData(b2ContactData* cts, int cap);
    b2MassData getMassData();
    void setMassData(b2MassData& data);
    
    // ---------- 碰撞回调 ----------
    //void setOnCollisionEnter(CollisionCallback callback);
    //void setOnCollisionExit(CollisionCallback callback);
    void onHitEvent(Collider* other,b2ContactHitEvent& hitevent);
    void onContactEventBegain(Collider* other);
    void onContactEventEnd(Collider* other);
    
    void onSensorEventBegain(Collider* other);
    void onSensorEventEnd(Collider* other);
    void setHitCallback(std::function<void(Collider* other, b2ContactHitEvent& hitevent)> callback);
    void setContactCallback();
    void setSensorCallback();
    //// 内部接口：由碰撞监听器调用
    //void onCollisionEnter(BoxCollider2D* other);
    //void onCollisionExit(BoxCollider2D* other);

    // ---------- 更新 ----------
    b2BodyId m_body;           // Box2D 刚体
    b2ShapeId m_shape;
protected:

    b2WorldId m_world;         // 物理世界指针
    bool m_isSensor = false;
    bool m_enabled = true;
    bool m_isBullet = false;
    bool m_allowRot = true;            //允许旋转

    std::function<void(Collider* other, b2ContactHitEvent& hitevent)> onHit;
    std::function<void(Collider* other)> onContactBegain;
    std::function<void(Collider* other)> onSensorBegain;
    std::function<void(Collider* other)> onContactEnd;
    std::function<void(Collider* other)> onSensorEnd;
    void destroy();
};
