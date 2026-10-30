#pragma once
#include "Collider.h"
class PolyCollider:public Collider
{
public:
    PolyCollider(b2WorldId world, const glm::vec2& position, const std::vector<glm::vec2>& points,
        const b2Vec2& offPos = {0,0}, float offAng = 0,
        float angle = 0.0f, bool isDynamic = true, bool isBullet = false, bool allowRot = true);

    PolyCollider(const PolyCollider&) = delete;
    PolyCollider& operator=(const PolyCollider&) = delete;
    std::vector<b2Vec2> pts;
    b2Vec2 offsetPos;
    float offsetAng;
    void update(); // 同步物理引擎状态到逻辑层（如有需要）
};

