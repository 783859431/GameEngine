#pragma once
#include "box2d/box2d.h"
b2DebugDraw drawFunc;

void DrawSegment(b2Vec2 p1, b2Vec2 p2, b2HexColor color, void* context);
void Draw() {
	drawFunc.DrawSegmentFcn = DrawSegment;
}
