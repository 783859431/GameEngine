#include "PolyCollider.h"

void PolyCollider::update()
{


}
PolyCollider::PolyCollider(b2WorldId world, const glm::vec2& position,const std::vector<glm::vec2>& points,
    const b2Vec2& offPos, float offAng, float angle , bool isDynamic, bool isBullet , bool allowRot)
    :Collider(world, position, angle, isDynamic, isBullet, allowRot)
{
    this->offsetAng = offAng;
    this->offsetPos = offPos;
    type = ColliderType::POLY;
    b2ShapeDef shape = b2DefaultShapeDef();
    shape.isSensor = this->m_isSensor;//是否作为传感器使用
    shape.userData = this;
    for (auto& it : points) {
        pts.push_back({ it.x,it.y });
    }
    b2Hull hull = b2ComputeHull(
        pts.data(),
        pts.size()
    );
    b2Polygon poly = b2MakeOffsetPolygon(&hull, offPos,b2MakeRot(glm::radians(offAng)));
    m_shape = b2CreatePolygonShape(m_body, &shape, &poly);

}