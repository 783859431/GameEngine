#include"WinAudioEngine.h"
#include"AudioDecoderFactory.h"
#include "alc.h"
#include "../logger/Logger.h"
#include "../utils/FileUtils.h"
#include "AudioEngine.h"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <winsock.h>
WinAudioEngine::WinAudioEngine()
    : _lazyInitLoop(true)
    , _currentAudioID(0)
{

}
static ALCdevice* s_ALDevice = nullptr;
static ALCcontext* s_ALContext = nullptr;
WinAudioEngine::~WinAudioEngine()
{


    if (s_ALContext) {
        alDeleteSources(MAX_AUDIOINSTANCES, _alSources);

        _audioCaches.clear();

        alcMakeContextCurrent(nullptr);
        alcDestroyContext(s_ALContext);
        s_ALContext = nullptr;
    }

    if (s_ALDevice) {
        alcCloseDevice(s_ALDevice);
        s_ALDevice = nullptr;
    }


}

bool WinAudioEngine::init()
{
    bool ret = false;

    s_ALDevice = alcOpenDevice(nullptr);//初始化OpenAL设备，参数为nullptr表示使用默认设备

    if (s_ALDevice) {
        alGetError();
        s_ALContext = alcCreateContext(s_ALDevice, nullptr);//创建上下文
        alcMakeContextCurrent(s_ALContext);//使用上下文

        alGenSources(MAX_AUDIOINSTANCES, _alSources);//创建资源
        auto alError = alGetError();
        if (alError != AL_NO_ERROR)
        {
            Logger::getInstance().error("%s:generating sources failed! error = %x\n", __FUNCTION__, alError);
            return ret;
        }

        for (int i = 0; i < MAX_AUDIOINSTANCES; ++i) {
            _alSourceUsed[_alSources[i]] = false;//将资源标记为空闲
        }
        ret = true;

    }


    return ret;
}

AudioData* WinAudioEngine::preload(const std::string& filePath, std::function<void(bool)> callback)
{
    AudioData* AudioData = nullptr;

    auto it = _audioCaches.find(filePath);//先找找cache里有没有这个文件的缓存
    if (it == _audioCaches.end()) {
        AudioData = &_audioCaches[filePath];
        AudioData->_fileFullPath =  FileUtils::GetInst()->GetFileFullPath(filePath);
        unsigned int cacheId = AudioData->_id;
        auto isCacheDestroyed = AudioData->_isDestroyed;
        //在别的线程去加载资源
        AudioEngine::addTask([AudioData, cacheId, isCacheDestroyed]() {
            if (isCacheDestroyed)
            {
                Logger::getInstance().info("AudioData (id=%u) was destroyed, no need to launch readDataTask.", cacheId);
                AudioData->setSkipReadDataTask(true);
                return;
            }
            AudioData->readDataTask(cacheId);//这个函数会在子线程里执行，读取数据并解析成pcm数据
            });
    }
    else {
        AudioData = &it->second;
    }

    //if (AudioData && callback)
    //{
    //    AudioData->addLoadCallback(callback);
    //}
    return AudioData;
}

int WinAudioEngine::play2d(const std::string& filePath, bool loop, float volume)
{
    if (s_ALDevice == nullptr) {
        return AudioEngine::INVALID_AUDIO_ID;
    }

    bool sourceFlag = false;
    ALuint alSource = 0;
    for (int i = 0; i < MAX_AUDIOINSTANCES; ++i) {//找一个空闲的source
        alSource = _alSources[i];

        if (!_alSourceUsed[alSource]) {
            sourceFlag = true;
            break;
        }
    }
    if (!sourceFlag) {
        return AudioEngine::INVALID_AUDIO_ID;
    }

    auto player = new (std::nothrow) AudioPlayer;
    if (player == nullptr) {
        return AudioEngine::INVALID_AUDIO_ID;
    }

    player->_alSource = alSource;
    player->_loop = loop;
    player->_volume = volume;

    auto AudioData = preload(filePath, nullptr);
    if (AudioData == nullptr) {
        delete player;
        return AudioEngine::INVALID_AUDIO_ID;
    }

    player->setCache(AudioData);
    _threadMutex.lock();
    _audioPlayers[_currentAudioID] = player;
    _threadMutex.unlock();

    _alSourceUsed[alSource] = true;

   AudioData->addPlayCallBack(std::bind(&WinAudioEngine::_play2d, this, AudioData, _currentAudioID));//

    if (_lazyInitLoop) {
        _lazyInitLoop = false;
        AudioEngine::addTask(std::bind(&WinAudioEngine::update, this, 0.05f));
    }

    return _currentAudioID++;
}

void WinAudioEngine::_play2d(AudioData* cache, int audioID)
{
    //Note: It may bn in sub thread or main thread :(
    if (!(cache->_isDestroyed) && cache->_state == AudioData::State::READY)
    {
        _threadMutex.lock();
        auto playerIt = _audioPlayers.find(audioID);
        if (playerIt != _audioPlayers.end() && playerIt->second->play2d()) {
            //_scheduler->performFunctionInCocosThread([audioID]() {

                if (AudioEngine::_audioIDInfoMap.find(audioID) != AudioEngine::_audioIDInfoMap.end()) {
                    AudioEngine::_audioIDInfoMap[audioID].state = AudioEngine::AudioState::PLAYING;
                }
            //    });
        }
        _threadMutex.unlock();
    }
    else
    {
        //ALOGD("WinAudioEngine::_play2d, cache was destroyed or not ready!");
        auto iter = _audioPlayers.find(audioID);
        if (iter != _audioPlayers.end())
        {
            iter->second->_removeByAudioEngine = true;
        }
    }
}

void WinAudioEngine::setVolume(int audioID, float volume)
{
    auto player = _audioPlayers[audioID];
    player->_volume = volume;

    if (player->_ready) {
        alSourcef(_audioPlayers[audioID]->_alSource, AL_GAIN, volume);

        auto error = alGetError();
        if (error != AL_NO_ERROR) {
             Logger::getInstance().error("%s: audio id = %d, error = %x", __FUNCTION__, audioID, error);
        }
    }
}

void WinAudioEngine::setLoop(int audioID, bool loop)
{
    auto player = _audioPlayers[audioID];

    if (player->_ready) {
        if (player->_streamingSource) {
            player->setLoop(loop);
        }
        else {
            if (loop) {
                alSourcei(player->_alSource, AL_LOOPING, AL_TRUE);
            }
            else {
                alSourcei(player->_alSource, AL_LOOPING, AL_FALSE);
            }

            auto error = alGetError();
            if (error != AL_NO_ERROR) {
                 Logger::getInstance().error("%s: audio id = %d, error = %x", __FUNCTION__, audioID, error);
            }
        }
    }
    else {
        player->_loop = loop;
    }
}

bool WinAudioEngine::pause(int audioID)
{
    bool ret = true;
    alSourcePause(_audioPlayers[audioID]->_alSource);

    auto error = alGetError();
    if (error != AL_NO_ERROR) {
        ret = false;
         Logger::getInstance().error("%s: audio id = %d, error = %x\n", __FUNCTION__, audioID, error);
    }

    return ret;
}

bool WinAudioEngine::resume(int audioID)
{
    bool ret = true;
    alSourcePlay(_audioPlayers[audioID]->_alSource);

    auto error = alGetError();
    if (error != AL_NO_ERROR) {
        ret = false;
         Logger::getInstance().error("%s: audio id = %d, error = %x\n", __FUNCTION__, audioID, error);
    }

    return ret;
}

void WinAudioEngine::stop(int audioID)
{
    auto player = _audioPlayers[audioID];
    player->destroy();
    //Note: Don't set the flag to false here, it should be set in 'update' function.
    // Otherwise, the state got from alSourceState may be wrong
//    _alSourceUsed[player->_alSource] = false;

    // Call 'update' method to cleanup immediately since the schedule may be cancelled without any notification.
    update(0.0f);
}

void WinAudioEngine::stopAll()
{
    for (auto&& player : _audioPlayers)
    {
        player.second->destroy();
    }
    //Note: Don't set the flag to false here, it should be set in 'update' function.
    // Otherwise, the state got from alSourceState may be wrong
//    for(int index = 0; index < MAX_AUDIOINSTANCES; ++index)
//    {
//        _alSourceUsed[_alSources[index]] = false;
//    }

    // Call 'update' method to cleanup immediately since the schedule may be cancelled without any notification.
    update(0.0f);
}

float WinAudioEngine::getDuration(int audioID)
{
    auto player = _audioPlayers[audioID];
    if (player->_ready) {
        return player->_audioCache->_duration;
    }
    else {
        return AudioEngine::TIME_UNKNOWN;
    }
}

float WinAudioEngine::getCurrentTime(int audioID)
{
    float ret = 0.0f;
    auto player = _audioPlayers[audioID];
    if (player->_ready) {
        if (player->_streamingSource) {
            ret = player->getTime();
        }
        else {
            alGetSourcef(player->_alSource, AL_SEC_OFFSET, &ret);

            auto error = alGetError();
            if (error != AL_NO_ERROR) {
                 Logger::getInstance().error("%s, audio id:%d,error code:%x", __FUNCTION__, audioID, error);
            }
        }
    }

    return ret;
}

bool WinAudioEngine::setCurrentTime(int audioID, float time)
{
    bool ret = false;
    auto player = _audioPlayers[audioID];

    do {
        if (!player->_ready) {
            break;
        }

        if (player->_streamingSource) {
            ret = player->setTime(time);
            break;
        }
        else {
            if (player->_audioCache->_framesRead != player->_audioCache->_totalFrames &&
                (time * player->_audioCache->_sampleRate) > player->_audioCache->_framesRead) {
                 Logger::getInstance().error("%s: audio id = %d", __FUNCTION__, audioID);
                break;
            }

            alSourcef(player->_alSource, AL_SEC_OFFSET, time);

            auto error = alGetError();
            if (error != AL_NO_ERROR) {
                 Logger::getInstance().error("%s: audio id = %d, error = %x", __FUNCTION__, audioID, error);
            }
            ret = true;
        }
    } while (0);

    return ret;
}

void WinAudioEngine::setFinishCallback(int audioID, const std::function<void(int, const std::string&)>& callback)
{
    _audioPlayers[audioID]->_finishCallbak = callback;
}

//移除已经完成的或者是需要被移除的音频
void WinAudioEngine::update(float dt)
{
    ALint sourceState;
    int audioID;
    AudioPlayer* player;
    ALuint alSource;

    //    Logger::getInstance().info("AudioPlayer count: %d", (int)_audioPlayers.size());
    do {


        for (auto it = _audioPlayers.begin(); it != _audioPlayers.end(); ) {//遍历所有正在播放的音频，检查是否有播放完成的，如果有则清理掉
            audioID = it->first;
            player = it->second;
            alSource = player->_alSource;
            alGetSourcei(alSource, AL_SOURCE_STATE, &sourceState);

            if (player->_removeByAudioEngine)
            {
                AudioEngine::remove(audioID);
                _threadMutex.lock();
                it = _audioPlayers.erase(it);
                _threadMutex.unlock();
                delete player;
                _alSourceUsed[alSource] = false;
            }
            else if (player->_ready && sourceState == AL_STOPPED) {//如果这个音频已经准备好了，并且状态是停止了，说明它播放完成了

                std::string filePath;
                if (player->_finishCallbak) {
                    auto& audioInfo = AudioEngine::_audioIDInfoMap[audioID];
                    filePath = audioInfo.filePath;
                }

                AudioEngine::remove(audioID);
                _threadMutex.lock();
                it = _audioPlayers.erase(it);
                _threadMutex.unlock();

                if (player->_finishCallbak) {
                    player->_finishCallbak(audioID, filePath); //FIXME: callback will delay 50ms
                }
                delete player;
                _alSourceUsed[alSource] = false;
            }
            else
            {
                ++it;
            }
        }

        if (_audioPlayers.empty()) {//如果没有正在播放的音频了，就停止调用update函数
            _lazyInitLoop = true;
            break;
        }
        Sleep(50);
    } while (true);



    //if (_audioPlayers.empty()) {//如果没有正在播放的音频了，就停止调用update函数
    //    _lazyInitLoop = true;
    //    _scheduler->unschedule(CC_SCHEDULE_SELECTOR(WinAudioEngine::update), this);
    //}
}

double WinAudioEngine::gettime()
{
    struct timeval val;
    LARGE_INTEGER liTime, liFreq;
    QueryPerformanceFrequency(&liFreq);
    QueryPerformanceCounter(&liTime);
    val.tv_sec = (long)(liTime.QuadPart / liFreq.QuadPart);
    val.tv_usec = (long)(liTime.QuadPart * 1000000.0 / liFreq.QuadPart - val.tv_sec * 1000000.0);
    return (double)val.tv_sec + (double)val.tv_usec / 1000000;
}

void WinAudioEngine::uncache(const std::string& filePath)
{
    _audioCaches.erase(filePath);
}

void WinAudioEngine::uncacheAll()
{
    _audioCaches.clear();
}