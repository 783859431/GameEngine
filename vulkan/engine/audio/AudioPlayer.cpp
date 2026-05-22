#include "AudioPlayer.h"
#include "../logger/Logger.h"
#include "AudioDecoderFactory.h"
#include <assert.h>
#define CHECK_AL_ERROR_DEBUG() \
do { \
    ALenum __error = alGetError(); \
    if (__error) { \
        Logger::getInstance().error("OpenAL error 0x%04X in %s %s %d\n", __error, __FILE__, __FUNCTION__, __LINE__); \
    } \
} while (false)
unsigned int __idIndex = 0;
AudioPlayer::AudioPlayer()
    : _audioCache(nullptr)
    , _finishCallbak(nullptr)
    , _isDestroyed(false)
    , _removeByAudioEngine(false)
    , _ready(false)
    , _currTime(0.0f)
    , _streamingSource(false)
    , _rotateBufferThread(nullptr)
    , _timeDirty(false)
    , _isRotateThreadExited(false)
    , _id(++__idIndex)
{
    memset(_bufferIds, 0, sizeof(_bufferIds));
}

AudioPlayer::~AudioPlayer()
{
    Logger::getInstance().info("~AudioPlayer() (%p), id=%u", this, _id);
    destroy();

    if (_streamingSource)
    {
        alDeleteBuffers(3, _bufferIds);
    }
}

void AudioPlayer::destroy()
{
    if (_isDestroyed)
        return;

    Logger::getInstance().info("AudioPlayer::destroy begin, id=%u", _id);

    _isDestroyed = true;

    do
    {
        if (_audioCache != nullptr)
        {
            if (_audioCache->_state == AudioData::State::INITIAL)
            {
                Logger::getInstance().info("AudioPlayer::destroy, id=%u, cache isn't ready!", _id);
                break;
            }

            while (!_audioCache->_isLoadingFinished)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
            }
        }

        // Wait for play2d to be finished.
        _play2dMutex.lock();
        _play2dMutex.unlock();

        if (_streamingSource)
        {
            if (_rotateBufferThread != nullptr)
            {
                while (!_isRotateThreadExited)
                {
                    _sleepCondition.notify_one();
                    std::this_thread::sleep_for(std::chrono::milliseconds(5));
                }

                if (_rotateBufferThread->joinable()) {
                    _rotateBufferThread->join();
                }

                delete _rotateBufferThread;
                _rotateBufferThread = nullptr;
                Logger::getInstance().info("rotateBufferThread exited!");
            }
        }
    } while (false);

    Logger::getInstance().info("Before alSourceStop");
    alSourceStop(_alSource); CHECK_AL_ERROR_DEBUG();
    Logger::getInstance().info("Before alSourcei");
    alSourcei(_alSource, AL_BUFFER, NULL); CHECK_AL_ERROR_DEBUG();

    _removeByAudioEngine = true;

    _ready = false;
    Logger::getInstance().info("AudioPlayer::destroy end, id=%u", _id);
}

void AudioPlayer::setCache(AudioData* cache)
{
    _audioCache = cache;
}

bool AudioPlayer::play2d()
{
    _play2dMutex.lock();
    Logger::getInstance().info("AudioPlayer::play2d, _alSource: %u, player id=%u", _alSource, _id);

    /*********************************************************************/
    /*       Note that it may be in sub thread or in main thread.       **/
    /*********************************************************************/
    bool ret = false;
    do
    {
        if (_audioCache->_state != AudioData::State::READY)
        {
            Logger::getInstance().error("alBuffer isn't ready for play!");
            break;
        }

        alSourcei(_alSource, AL_BUFFER, 0); CHECK_AL_ERROR_DEBUG();// unbind buffer, just in case the buffer is still bound to source when play2d is called again after stop.
        alSourcef(_alSource, AL_PITCH, 1.0f); CHECK_AL_ERROR_DEBUG();// 播放速度
        alSourcef(_alSource, AL_GAIN, _volume); CHECK_AL_ERROR_DEBUG();// 音量
        alSourcei(_alSource, AL_LOOPING, AL_FALSE); CHECK_AL_ERROR_DEBUG();

        if (_audioCache->_queBufferFrames == 0)
        {
            if (_loop) {
                alSourcei(_alSource, AL_LOOPING, AL_TRUE);
                CHECK_AL_ERROR_DEBUG();
            }
        }
        else
        {
            alGenBuffers(3, _bufferIds);

            auto alError = alGetError();
            if (alError == AL_NO_ERROR)
            {
                for (int index = 0; index < QUEUEBUFFER_NUM; ++index)
                {
                    alBufferData(_bufferIds[index], _audioCache->_format, _audioCache->_queBuffers[index], _audioCache->_queBufferSize[index], _audioCache->_sampleRate);//写入音频
                }
                CHECK_AL_ERROR_DEBUG();
            }
            else
            {
                Logger::getInstance().error("%s:alGenBuffers error code:%x", __FUNCTION__, alError);
                break;
            }
            _streamingSource = true;
        }

        {
            std::unique_lock<std::mutex> lk(_sleepMutex);
            if (_isDestroyed)
                break;

            if (_streamingSource)
            {
                alSourceQueueBuffers(_alSource, QUEUEBUFFER_NUM, _bufferIds);
                CHECK_AL_ERROR_DEBUG();
                _rotateBufferThread = new std::thread(&AudioPlayer::rotateBufferThread, this, _audioCache->_queBufferFrames * QUEUEBUFFER_NUM + 1);
            }
            else
            {
                alSourcei(_alSource, AL_BUFFER, _audioCache->_alBufferId);
                CHECK_AL_ERROR_DEBUG();
            }

            alSourcePlay(_alSource);
        }

        auto alError = alGetError();
        if (alError != AL_NO_ERROR)
        {
            Logger::getInstance().error("%s:alSourcePlay error code:%x", __FUNCTION__, alError);
            break;
        }

        ALint state;
        alGetSourcei(_alSource, AL_SOURCE_STATE, &state);
        if (state != AL_PLAYING)
        {
            Logger::getInstance().error("state isn't playing, %d, %s, cache id=%u, player id=%u", state, _audioCache->_fileFullPath.c_str(), _audioCache->_id, _id);
        }
        (state == AL_PLAYING);
        _ready = true;
        ret = true;
    } while (false);

    if (!ret)
    {
        _removeByAudioEngine = true;
    }

    _play2dMutex.unlock();
    return ret;
}

void AudioPlayer::rotateBufferThread(int offsetFrame)
{
    char* tmpBuffer = nullptr;
    AudioDecoder* decoder = AudioDecoderFactory::createDecoder(_audioCache->_fileFullPath.c_str());
    do
    {
        if (decoder == nullptr || !decoder->open(_audioCache->_fileFullPath.c_str()))
            break;
        uint32_t framesRead = 0;
        const uint32_t framesToRead = _audioCache->_queBufferFrames;
        const uint32_t bufferSize = framesToRead * decoder->getBytesPerFrame();
        tmpBuffer = (char*)malloc(bufferSize);
        memset(tmpBuffer, 0, bufferSize);

        if (offsetFrame != 0) {
            decoder->seek(offsetFrame);
        }

        ALint sourceState;
        ALint bufferProcessed = 0;
        bool needToExitThread = false;

        while (!_isDestroyed) {
            alGetSourcei(_alSource, AL_SOURCE_STATE, &sourceState);
            if (sourceState == AL_PLAYING) { // source is playing, check if any buffer is processed
                alGetSourcei(_alSource, AL_BUFFERS_PROCESSED, &bufferProcessed);//空闲的buffer数量
                while (bufferProcessed > 0) {
                    bufferProcessed--;
                    if (_timeDirty) {// seek to the new position if time is dirty
                        _timeDirty = false;
                        offsetFrame = (int)(_currTime * decoder->getSampleRate());
                        decoder->seek(offsetFrame);
                    }
                    else {
                        _currTime += QUEUEBUFFER_TIME_STEP;
                        if (_currTime > _audioCache->_duration) {
                            if (_loop) {
                                _currTime = 0.0f;
                            }
                            else {
                                _currTime = _audioCache->_duration;
                            }
                        }
                    }

                    framesRead = decoder->readFixedFrames(framesToRead, tmpBuffer);

                    if (framesRead == 0) {//没有音频数据了
                        if (_loop) {
                            decoder->seek(0);
                            framesRead = decoder->readFixedFrames(framesToRead, tmpBuffer);
                        }
                        else {
                            needToExitThread = true;
                            break;
                        }
                    }
                    //填充新的音频数据
                    ALuint bid;
                    alSourceUnqueueBuffers(_alSource, 1, &bid);
                    alBufferData(bid, _audioCache->_format, tmpBuffer, framesRead * decoder->getBytesPerFrame(), decoder->getSampleRate());
                    alSourceQueueBuffers(_alSource, 1, &bid);
                }
            }

            std::unique_lock<std::mutex> lk(_sleepMutex);
            if (_isDestroyed || needToExitThread) {
                break;
            }

            _sleepCondition.wait_for(lk, std::chrono::milliseconds(75));
        }

    } while (false);

    Logger::getInstance().info("Exit rotate buffer thread ...");
    if (decoder != nullptr)
    {
        decoder->close();
    }
    AudioDecoderFactory::destroyDecoder(decoder);
    free(tmpBuffer);
    _isRotateThreadExited = true;
    Logger::getInstance().info("%s exited.\n", __FUNCTION__);
}
//设置循环播放
bool AudioPlayer::setLoop(bool loop)
{
    if (!_isDestroyed) {
        _loop = loop;
        return true;
    }

    return false;
}
//设置音频播放位置
bool AudioPlayer::setTime(float time)
{
    if (!_isDestroyed && time >= 0.0f && time < _audioCache->_duration) {

        _currTime = time;
        _timeDirty = true;

        return true;
    }
    return false;
}