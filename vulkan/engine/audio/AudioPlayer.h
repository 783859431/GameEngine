#pragma once
#include <al.h>
#include <string>
#include <functional>
#include <thread>
#include <mutex>
#include "AudioData.h"

class AudioPlayer
{

public:
    AudioPlayer();
    ~AudioPlayer();

    void destroy();

    //queue buffer related stuff
    bool setTime(float time);
    float getTime() { return _currTime; }
    bool setLoop(bool loop);

protected:
    void setCache(AudioData* cache);
    void rotateBufferThread(int offsetFrame);
    bool play2d();

    AudioData* _audioCache;

    float _volume;
    bool _loop;
    std::function<void(int, const std::string&)> _finishCallbak;

    bool _isDestroyed;
    bool _removeByAudioEngine;
    bool _ready;
    ALuint _alSource;

    //play by circular buffer
    float _currTime;
    bool _streamingSource;
    ALuint _bufferIds[3];
    std::thread* _rotateBufferThread;
    std::condition_variable _sleepCondition;
    std::mutex _sleepMutex;
    bool _timeDirty;
    bool _isRotateThreadExited;

    std::mutex _play2dMutex;

    unsigned int _id;
 
    friend class WinAudioEngine;
};

