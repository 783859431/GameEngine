#include "Collider.h"



void Collider::createBody(bool isDynamic,const glm::vec2& position, float angle)
{
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.fixedRotation = !m_allowRot;
    bodyDef.isBullet = m_isBullet;
    bodyDef.userData = this;
    bodyDef.type = isDynamic ? b2_dynamicBody : b2_staticBody;
    bodyDef.position = { position.x,position.y };
 
    float r = glm::radians(angle);
    bodyDef.rotation = { glm::cos(r), glm::sin(r) };
    m_body = b2CreateBody(m_world, &bodyDef);
}

Collider::Collider(b2WorldId world, const glm::vec2& position, float angle, bool isDynamic, bool isBullet, bool allowRot)
    :m_world(world), m_isBullet(isBullet), m_allowRot(allowRot), m_body(),m_shape()
{
    createBody(isDynamic,position,angle);
}

Collider::~Collider()
{
    destroy();
}


// ---------- 物理属性设置 ----------
void Collider::setTransform(const glm::vec2& position, float angle) {

    float r = glm::radians(angle);
    b2Body_SetTransform(m_body, { position.x,position.y }, { cosf(r),sinf(r) });
}


void Collider::setDensity(float density) {

    b2Shape_SetDensity(m_shape, density, true);

}

void Collider::setFriction(float friction) {

    b2Shape_SetFriction(m_shape, friction);

}

//恢复系数如果是1就是完全弹性碰撞
void Collider::setRestitution(float restitution) {

    b2Shape_SetRestitution(m_shape, restitution);
}

void Collider::setGravityScale(float gs)
{
    b2Body_SetGravityScale(m_body, gs);

}


void Collider::setEnabled(bool enabled) {
    m_enabled = enabled;
    if (enabled)
        b2Body_Enable(m_body);
    else
        b2Body_Disable(m_body);
}

void Collider::setLinearVelocity(const glm::vec2& v)
{
    b2Body_SetLinearVelocity(m_body, { v.x,v.y });
}

void Collider::setLinearDamping(float x)
{
    b2Body_SetLinearDamping(m_body, x);
}

void Collider::setAngularDamping(float x)
{
    b2Body_SetAngularDamping(m_body, x);
}

void Collider::setColliderFilter(uint32_t category, uint32_t mask, int groupIndex)
{
    b2Filter filter;
    filter.categoryBits = category;
    filter.maskBits = mask;
    filter.groupIndex = groupIndex;
    b2Shape_SetFilter(m_shape, filter);
}

void Collider::applyForce(const glm::vec2& force)
{
    b2Body_ApplyForceToCenter(m_body, { force.x,force.y }, true);
}

void Collider::applyForce(const glm::vec2& force, const glm::vec2& point)
{

    b2Body_ApplyForce(m_body, { force.x,force.y }, { point.x,point.y }, true);
}

void Collider::applyTorque(float torque)
{
    b2Body_ApplyTorque(m_body, torque, true);
}

void Collider::enableSensorEvent(bool flag)
{
    b2Shape_EnableSensorEvents(m_shape,flag);
}



void Collider::setAngularVelocity(float radiansPersecond)
{
    b2Body_SetAngularVelocity(m_body, radiansPersecond);
}

void Collider::enableHitEvent(bool flag)
{
    b2Shape_EnableHitEvents(m_shape, flag);
}

void Collider::enablePreSolveEvent(bool flag)
{
    b2Shape_EnablePreSolveEvents(m_shape, flag);
}

void Collider::enableContactEvent(bool flag)
{
    b2Shape_EnableContactEvents(m_shape, flag);
}

// ---------- 获取数据 ----------
glm::vec2 Collider::getPosition() const {
    b2Vec2 pos = b2Body_GetPosition(m_body);
    return glm::vec2(pos.x, pos.y);
}

float Collider::getAngle() const
{
    b2Rot rot = b2Body_GetRotation(m_body);

    return glm::degrees(glm::acos(rot.c));
}

glm::vec2 Collider::getRot() const
{
    b2Rot rot = b2Body_GetRotation(m_body);

    return glm::vec2(rot.c, rot.s);
}



bool Collider::isSensor() const { return m_isSensor; }

bool Collider::isEnabled() const
{
    return m_enabled;
}

int Collider::getContactData(b2ContactData* cts, int cap)
{

    int count = b2Shape_GetContactData(m_shape, cts, cap);

    return count;
}

b2MassData Collider::getMassData()
{
    return  b2Shape_GetMassData(m_shape);
}

void Collider::setMassData(b2MassData& data)
{
    b2Body_SetMassData(m_body,data);
}

void Collider::onHitEvent(Collider* other, b2ContactHitEvent& hitevent)
{
    if (onHit!=nullptr)
        onHit(other,hitevent);

}

void Collider::onContactEventBegain(Collider* other)
{
    if (onContactBegain)
    {
        onContactBegain(other);
    }
}

void Collider::onContactEventEnd(Collider* other)
{
    if (onContactEnd)
    {
        onContactEnd(other);
    }

}

void Collider::onSensorEventBegain(Collider* other)
{
    if (onSensorBegain)
    {
        onSensorBegain(other);
    }
}

void Collider::onSensorEventEnd(Collider* other)
{
    if (onSensorEnd)
    {
        onSensorEnd(other);
    }

}




void Collider::setHitCallback(std::function<void(Collider* other, b2ContactHitEvent& hitevent)> callback)
{
    this->onHit = callback;

}

void Collider::setContactCallback()
{


}

void Collider::setSensorCallback()
{

}

void Collider::destroy()
{
    if(b2Body_IsValid(m_body))
        b2DestroyBody(m_body);

}
