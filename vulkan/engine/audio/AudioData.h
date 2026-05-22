#pragma once
#include <al.h>
#include <string>
#include <functional>
#include <mutex>

#define INVALID_AL_BUFFER_ID 0xFFFFFFFF
#define PCMDATA_CACHEMAXSIZE 1048576

#define QUEUEBUFFER_NUM (3)
#define QUEUEBUFFER_TIME_STEP (0.1f)
class AudioData
{
public:

    enum class State
    {
        INITIAL,
        LOADING,
        READY,
        FAILED
    };

    void addPlayCallBack(std::function<void()>call);
protected:
    void setSkipReadDataTask(bool isSkip) { _isSkipReadDataTask = isSkip; };
    void readDataTask(unsigned int selfId);
    std::mutex _playCallbackMutex;
    std::vector< std::function<void()>> _playCallbacks;
    void invokingPlayCallbacks();
    
    std::mutex _readDataTaskMutex;

    ALenum _format;
    ALsizei _sampleRate;
    float _duration;
    uint32_t _totalFrames;
    uint32_t _framesRead;

    /*Cache related stuff;
     * Cache pcm data when sizeInBytes less than PCMDATA_CACHEMAXSIZE
     */
    //当音频字节数少，时使用
    ALuint _alBufferId;
    char* _pcmData;

    /*Queue buffer related stuff
     *  Streaming in OpenAL when sizeInBytes greater then PCMDATA_CACHEMAXSIZE
     */
    //当音频字节数多时采用流式播放
    char* _queBuffers[QUEUEBUFFER_NUM];
    ALsizei _queBufferSize[QUEUEBUFFER_NUM];
    uint32_t _queBufferFrames;


    State _state;

    bool _isDestroyed;
    std::string _fileFullPath;
    unsigned int _id;
    bool _isLoadingFinished;
    bool _isSkipReadDataTask;
    friend class AudioPlayer;
    friend class WinAudioEngine;
};

