#pragma once
#include "Collider.h"
class BallCollider2D:public Collider  
{
public:
	BallCollider2D(b2WorldId world, const glm::vec2& position, float radius,
		float angle = 0.0f, bool isDynamic = true, bool isBullet = false, bool allowRot = true
		);

	void setRadius(float radius);
	float getRadius() { return radius; }
	
	float radius = 0;
};

