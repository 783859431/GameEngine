#pragma once
#include <string>
#include <functional>
#include "AudioData.h"
 class IAudioEngine
{
public:
    //初始化
    virtual bool init() = 0;
    //播放声音
    virtual int play2d(const std::string& fileFullPath, bool loop, float volume) =0;
    //设置音量
    virtual void setVolume(int audioID, float volume) = 0;
    //循环播放
    virtual void setLoop(int audioID, bool loop) =0;
    //暂停
    virtual bool pause(int audioID) =0;
    //继续
    virtual bool resume(int audioID) =0;
    //停止
    virtual void stop(int audioID) =0;
    virtual void stopAll() =0;
    //获取时长
    virtual float getDuration(int audioID) =0;
    //获取当前时长
    virtual float getCurrentTime(int audioID) =0;
    //设置当前时长
    virtual bool setCurrentTime(int audioID, float time) =0;
    //设置播放完成回调
    virtual void setFinishCallback(int audioID, const std::function<void(int, const std::string&)>& callback) =0;
    //将文件从缓冲移除
    virtual void uncache(const std::string& filePath) =0;
    virtual void uncacheAll() = 0;
    //加载文件
    virtual AudioData* preload(const std::string& filePath, std::function<void(bool)> callback) = 0;
    //持续播放
    virtual void update(float dt) =0;
    //
    virtual double gettime() = 0;

};

