#include "BallCollider2D.h"
BallCollider2D::BallCollider2D(b2WorldId world, const glm::vec2& position, float radius,
	float angle, bool isDynamic , bool isBullet, bool allowRot):
	Collider(world,position,angle,isDynamic,isBullet,allowRot)
{
    type = ColliderType::BALL;
    b2Circle circle;
    circle.center = { 0.0f, 0.0f };
    circle.radius = radius;

    b2ShapeDef shape = b2DefaultShapeDef();
    shape.isSensor = this->m_isSensor;//是否作为传感器使用
    shape.userData = this;
    m_shape = b2CreateCircleShape(m_body, &shape, &circle);

}

void BallCollider2D::setRadius(float radius)
{
    this->radius = radius;
    b2Circle circle;
    circle.center = { 0.0f, 0.0f };
    circle.radius = radius;
    b2Shape_SetCircle(m_shape, &circle);
   
}
