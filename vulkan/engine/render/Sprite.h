#pragma once
#include "PolygonRender.h"
#include "Texture.h"
#include "../Global.h"
struct  Rect
{
	float x;//图片左上角坐标
	float y;
	float w;
	float h;
	Rect()
	{
		x = y = w = h=0;
	}
	Rect(float _x, float _y, float _w, float _h) {
		x = _x;
		y = _y;
		w = _w;
		h = _h;
	}
	
};


//表示一张图片，可以改变位置，旋转和缩放。
class Sprite
{
public:
	Texture* texture = 0; // 一张Texture中可能包含着多个Sprite
	Transform2d transform; // 图像的位置和旋转缩放
	Rect rawPos; // 原始图片的像素坐标，正常情况下（0，0）为图片的左上角，在图形采样时只采样在rawPos中的坐标。
	float w = 0; // 图像的实际宽度（像素）
	float h = 0; // 图像的实际高度 （像素）
	void* obj;
	Sprite(const Rect& info)
	{
		this->rawPos = info;
		this->w = info.w;
		this->h = info.h;
		
	}
	Sprite() {

	}

	void setTexture(Texture* tex)
	{
		this->texture = tex;
		rawPos = { 0.0,0.0,(float)tex->width,(float) tex->height };
		w = tex->width;
		h = tex->height;
	}
	void setRawPos(const Rect& info)
	{
		this->rawPos = info;
	}
	void setWH(float w, float h)
	{
		this->w = w;
		this->h = h;
	}
};
