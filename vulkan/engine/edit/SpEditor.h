#pragma once
#include <vector>
#include "../render/Sprite.h"
#include "../input/Input.h"
#include "../render/camera/Camera.h"
#include "../render/PolygonRender.h"
static bool PointInRect(const glm::vec2& pt, Transform2d* transform, int w, int h);
class SpEditor
{

	std::vector<Sprite*> sps;

	float selw;
	float selh;
	glm::vec2 arr[5];
	int controlPoint = -1;
	glm::vec2 mouseStartPos;
public:
	Sprite* sel;
	Sprite* selected(Camera& cam);
	void Add(Sprite* sp) {
		sps.push_back(sp);
	}
	void Del(Sprite* sp)
	{
		sps.erase(std::remove(sps.begin(),sps.end(),sp),sps.end());
		if (sp == sel)
			sel = 0;
	}
	void MoveSel(Camera& cam,float dt);

	void ControlPoints(PolygonRender& render);
};

