#pragma once
#include "IAudioEngine.h"
#include "AudioPlayer.h"
class AudioEngine;
class WinAudioEngine:public IAudioEngine
{

#define MAX_AUDIOINSTANCES 32

    public:
        WinAudioEngine();
        ~WinAudioEngine();
        //初始化
        bool init();
        //播放声音
        int play2d(const std::string& fileFullPath, bool loop, float volume);
        //设置音量
        void setVolume(int audioID, float volume);
        //循环播放
        void setLoop(int audioID, bool loop);
        //暂停
        bool pause(int audioID);
        //继续
        bool resume(int audioID);
        //停止
        void stop(int audioID);
        void stopAll();
        //获取时长
        float getDuration(int audioID);
        //获取当前时长
        float getCurrentTime(int audioID);
        //设置当前时长
        bool setCurrentTime(int audioID, float time);
        //设置播放完成回调
        void setFinishCallback(int audioID, const std::function<void(int, const std::string&)>& callback);
        //将文件从缓冲移除
        void uncache(const std::string& filePath);
        void uncacheAll();
        //加载文件
        AudioData* preload(const std::string& filePath, std::function<void(bool)> callback);
        //持续播放
        void update(float dt);
        double gettime();
    private:
        void _play2d(AudioData* cache, int audioID);

        ALuint _alSources[MAX_AUDIOINSTANCES];

        //source,used
        std::unordered_map<ALuint, bool> _alSourceUsed;

        //filePath,bufferInfo
        std::unordered_map<std::string, AudioData> _audioCaches;

        //audioID,AudioInfo
        std::unordered_map<int, AudioPlayer*>  _audioPlayers;
        std::mutex _threadMutex;

        bool _lazyInitLoop;

        int _currentAudioID;

  

};