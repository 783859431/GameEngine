#include "SpEditor.h"

static bool PointInRect(const glm::vec2& pt, Transform2d* transform, int w, int h) {
	glm::vec2 p2 = pt - glm::vec2(transform->x, transform->y);
	float x = p2.x * transform->c + p2.y * transform->s;
	float y = -p2.x * transform->s + p2.y * transform->c;
	return std::abs(x) <= w * 0.5f && std::abs(y) <= h * 0.5f;
}
static bool PointInCircle(const glm::vec2& pt, const glm::vec2& c, float r) {
	auto ds = pt - c;
	return glm::length(ds) < r;
}
Sprite* SpEditor::selected(Camera& cam) {
	glm::vec2 mousePos = glm::vec2(Input::Mouse::x, Input::Mouse::y);
	glm::vec3 world;
	ScreenToWorld(cam, mousePos, world);
	for (auto it : sps)
	{
		bool  res = PointInRect(world, &it->transform, PixelToUnit(it->w), PixelToUnit(it->h));
		if (res) {
			return it;
		}
	}
	return nullptr;
}

void SpEditor::MoveSel(Camera& cam,float dt) 
{
	if (Input::Mouse::MouseButtonPressed(Input::LeftButton))
	{
		if (sel) //上一帧已经选中了物体，判断一下是否落在其控制点内
		{
			glm::vec2 mousePos = glm::vec2(Input::Mouse::x, Input::Mouse::y);
			glm::vec3 world;
			ScreenToWorld(cam, mousePos, world);
			controlPoint = -1;
			for (int i = 0; i <= 4; i++) {

				bool res = PointInCircle(world, arr[i], 0.5);
				if (res) {
					controlPoint = i;
					break;
				}
			}
			if (controlPoint == -1)
			{
				sel = selected(cam);
				
			}
		
		}
		else
		{
			sel = selected(cam);

		  
		}
		mouseStartPos = { Input::Mouse::x,Input::Mouse::y };
		//
	}

	if (Input::Mouse::MouseButtonDown(Input::LeftButton)) {
		if (sel)
		{
			if (controlPoint <=3&&controlPoint>=0)
			{

				glm::vec2 mouseOffset = { Input::Mouse::x - mouseStartPos.x,Input::Mouse::y - mouseStartPos.y};
				glm::vec2 dir = { sel->transform.x - arr[controlPoint].x ,sel->transform.y - arr[controlPoint].y };
				dir = glm::normalize(dir);
				float a = glm::dot(dir, mouseOffset);
				float scale = a * dt*-1;
				if (scale <= -1) scale = -0.9;

				float newWidth = selw * (1 + scale);
				float newHeight = selh * (1 + scale);

				//左上角坐标
				float x = arr[3 - controlPoint].x;
				float y = arr[3 - controlPoint].y;
	
				float oldX = x;
				float oldY = y;
                //反向旋转，使得矩形水平
				x = oldX * sel->transform.c + oldY * sel->transform.s;
				y = -oldX * sel->transform.s + oldY * sel->transform.c;

				//根据左上角反算中心坐标
				if (controlPoint == 3)
				{
					x = x + PixelToUnit(newWidth) * 0.5f;
					y = y + PixelToUnit(newHeight) * 0.5f;
				}
				else if (controlPoint == 0)
				{
					x = x- PixelToUnit(newWidth) * 0.5f;
					y = y - PixelToUnit(newHeight) * 0.5f;


				}
				else if (controlPoint == 1)
				{
					x = x - PixelToUnit(newWidth) * 0.5f;
					y = y + PixelToUnit(newHeight) * 0.5f;
				}
				else if (controlPoint == 2)
				{
					x = x + PixelToUnit(newWidth) * 0.5f;
					y = y - PixelToUnit(newHeight) * 0.5f;
				}

				//恢复旋转角
				sel->transform.x = x * sel->transform.c - y * sel->transform.s;
				sel->transform.y = x * sel->transform.s + y * sel->transform.c;
				sel->w = newWidth;
				sel->h = newHeight;
			}
			else if (controlPoint == 4)
			{
				//旋转物体

				float dx =  (Input::Mouse::x - mouseStartPos.x)*dt;
				sel->transform.c = glm::cos(dx);
				sel->transform.s = glm::sin(dx);

			}
			else
			{
				glm::vec2 mousePos = glm::vec2(Input::Mouse::x, Input::Mouse::y);
				glm::vec3 world;
				ScreenToWorld(cam, mousePos, world);
				sel->transform.x = world.x;
				sel->transform.y = world.y;
			}

		}
	}

	if (Input::Mouse::MouseButtonReleased(Input::LeftButton))
	{
		if (sel)
		{
			selw = sel->w;
			selh = sel->h;
		}
	}
}

void SpEditor::ControlPoints(PolygonRender& render) {
	if (sel) {
		float w = PixelToUnit(sel->w);
		float h = PixelToUnit(sel->h);
		arr[0] = { -w / 2,-h / 2 };
		arr[1] = { -w / 2,h / 2 };
		arr[2] = { w / 2,-h / 2 };
		arr[3] = { w / 2,h / 2 };
		arr[4] = { 0, -h / 2 - 1 };

		for (int i = 0; i <= 4; i++)
		{
			float x = arr[i].x;
			float y = arr[i].y;
			arr[i].x = sel->transform.x + x * sel->transform.c - y * sel->transform.s;
			arr[i].y = sel->transform.y + x * sel->transform.s + y * sel->transform.c;
			render.drawCircle({ arr[i].x,arr[i].y,1,0 }, 0.5, { 1,0,0,1 });
		}

	
	}
	
}