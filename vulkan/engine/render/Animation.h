#pragma once
#include <vector>
#include <functional>
#include <memory>
#include <string>
#include "Sprite.h"

//通过修改精灵图采样位置实现动画效果
class AnimationSprite:public Sprite
{	
	float sheetW;
	float sheetH;
	float timeCount =0;
	float frameTime = 0;
	int  fps = 1;
	bool isLoop = true;
	bool isPause = false;
	bool isEnd = false;


public:
	std::string name;
	std::vector<Rect> animaFrames;
	int currentFrameIndex = 0;
	std::function<void()> OnAnimationEnd;
	void setFps(float fps) {
		this->fps = fps;
		frameTime = 1.0 / fps;
	}
	void setPause(bool pause)
	{
		isPause = pause;
	}
	void setLoop(bool loop)
	{
		this->isLoop = loop;
	}
	void update(float dt)
	{
		if (isPause || isEnd) return;
		if (animaFrames.size() == 0)return;
		timeCount += dt;
		if (timeCount > frameTime)
		{
			timeCount = 0;
		    currentFrameIndex++;
			if (currentFrameIndex == animaFrames.size())
			{
				if (isLoop)
				{
					currentFrameIndex = 0;
				}
				else
				{
					isEnd = true;
					currentFrameIndex--;
					//执行某个回调
				}

			}
			this->rawPos = animaFrames[currentFrameIndex];
			
		}
	}

};


//class SheetAnimationPlayer
//{
//
//	std::unordered_map<std::string, std::shared_ptr<SheetAnimation>> animas;
//	//
//	//
//	void update(float dt)
//	{
//		for (auto it = animas.begin(); it != animas.end(); ++it)
//		{
//			it->second->update(dt);
//		}
//
//	}
//	void play(std::string& name)
//	{
//		if (animas.count(name) != 0)
//		{
//			animas[name]->setPause(false);
//			return;
//		}
//		std::shared_ptr<SheetAnimation> ani;
//		bool res = AnimationCache::find(name,ani);
//		if (res)
//		{
//			ani->setPause(false);
//            animas[name] = ani;
//			
//		}
//			
//	}
//
//	void pause(std::string& name)
//	{
//		if (animas.count(name) != 0)
//		{
//			animas[name]->setPause(true);
//		}
//	}
//
//};
//
//class  AnimationCache
//{
//public:
//	static std::unordered_map<std::string, std::shared_ptr<SheetAnimation>> animas;
//	static bool find(std::string& name, std::shared_ptr<SheetAnimation>& anima) {
//		if (animas.find(name) != animas.end())
//		{
//			anima = animas[name];
//			return true;
//		}
//		return false;
//	}
//};