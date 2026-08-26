#include "BoxCollider2D.h"

// ---------- 构造函数 ----------


BoxCollider2D::BoxCollider2D(b2WorldId world, const glm::vec2& position, const glm::vec2& size, float angle, 
    bool isDynamic,
    bool isBullet, bool allowRot)
    :Collider(world,position,angle,isDynamic, isBullet, allowRot)
{
    type = ColliderType::BOX;
    b2ShapeDef shape = b2DefaultShapeDef();
    shape.isSensor = this->m_isSensor;//是否作为传感器使用
    shape.userData = this;
    b2Polygon poly = b2MakeBox(size.x/2,size.y/2);
    m_shape = b2CreatePolygonShape(m_body, &shape, &poly);
}





void BoxCollider2D::setSize(const glm::vec2& size) {
    m_size = size;
    b2Polygon poly = b2MakeBox(size.x, size.y);
    b2Shape_SetPolygon(m_shape, &poly);
}

glm::vec2 BoxCollider2D::getSize() const
{
    return this->m_size;
}




void BoxCollider2D::update() {
    // 如果有需要同步的渲染属性，在这里处理
}

