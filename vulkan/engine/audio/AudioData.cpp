#include "AudioData.h"
#include <vector>
#include "AudioDecoderFactory.h"
#include "../logger/Logger.h"
void AudioData::addPlayCallBack(std::function<void()> call)
{

    std::lock_guard<std::mutex> lk(_playCallbackMutex);
    switch (_state)
    {
    case State::INITIAL:
    case State::LOADING:
        _playCallbacks.push_back(call);
        break;
    
    case State::READY:
        // If state is failure, we still need to invoke the callback
        // since the callback will set the 'AudioPlayer::_removeByAudioEngine' flag to true.
    case State::FAILED:
        call();
        break;
    
    default:
        Logger::getInstance().error("Invalid state: %d", _state);
        break;
    }

}
void AudioData::readDataTask(unsigned int selfId)
{
    std::scoped_lock lock(_readDataTaskMutex);
    _state = State::LOADING;

    AudioDecoder* decoder = AudioDecoderFactory::createDecoder(_fileFullPath.c_str());

    if (decoder == nullptr || !decoder->open(_fileFullPath.c_str()))
        return;

    const uint32_t originalTotalFrames = decoder->getTotalFrames();//
    const uint32_t bytesPerFrame = decoder->getBytesPerFrame();
    const uint32_t sampleRate = decoder->getSampleRate();
    const uint32_t channelCount = decoder->getChannelCount();

    uint32_t totalFrames = originalTotalFrames;
    uint32_t dataSize = totalFrames * bytesPerFrame;
    uint32_t remainingFrames = totalFrames;
    uint32_t adjustFrames = 0;

    _format = channelCount > 1 ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16;
    _sampleRate = (ALsizei)sampleRate; // 采样速率
    _duration = 1.0f * totalFrames / sampleRate; //
    _totalFrames = totalFrames; //总帧数量

    if (dataSize <= PCMDATA_CACHEMAXSIZE)//音频是小文件时
    {
        uint32_t framesRead = 0;
        const uint32_t framesToReadOnce = std::min(totalFrames, static_cast<uint32_t>(sampleRate * QUEUEBUFFER_TIME_STEP * QUEUEBUFFER_NUM));

        std::vector<char> adjustFrameBuf;

        if (decoder->seek(totalFrames))
        {
            char* tmpBuf = (char*)malloc(framesToReadOnce * bytesPerFrame);
            adjustFrameBuf.reserve(framesToReadOnce * bytesPerFrame);

            // Adjust total frames by setting position to the end of frames and try to read more data.
            // This is a workaround for https://github.com/cocos2d/cocos2d-x/issues/16938

            do
            {
                framesRead = decoder->read(framesToReadOnce, tmpBuf);
                if (framesRead > 0)
                {
                    adjustFrames += framesRead;
                    adjustFrameBuf.insert(adjustFrameBuf.end(), tmpBuf, tmpBuf + framesRead * bytesPerFrame);
                }

            } while (framesRead > 0);//音频插入末尾的几个字节

            if (adjustFrames > 0)
            {
                Logger::getInstance().info("Orignal total frames: %u, adjust frames: %u, current total frames: %u", totalFrames, adjustFrames, totalFrames + adjustFrames);
                totalFrames += adjustFrames;
                _totalFrames = remainingFrames = totalFrames;
            }

            // Reset dataSize
            dataSize = totalFrames * bytesPerFrame;

            free(tmpBuf);
        }

        if (!decoder->seek(0))
        {
            Logger::getInstance().error("%s: AudioDecoder::seek(0) failed!", __FUNCTION__);
			return;
        }

        _pcmData = (char*)malloc(dataSize);
        memset(_pcmData, 0x00, dataSize);

        if (adjustFrames > 0)
        {
            memcpy(_pcmData + (dataSize - adjustFrameBuf.size()), adjustFrameBuf.data(), adjustFrameBuf.size());
        }

        alGenBuffers(1, &_alBufferId);
        auto alError = alGetError();
        if (alError != AL_NO_ERROR) {
            Logger::getInstance().error("%s: attaching audio to buffer fail: %x", __FUNCTION__, alError);
            return;
        }

        if (_isDestroyed)
            return;

        framesRead = decoder->readFixedFrames(std::min(framesToReadOnce, remainingFrames), _pcmData + _framesRead * bytesPerFrame);
        _framesRead += framesRead;
        remainingFrames -= framesRead;

        if (_isDestroyed)
            return;

        uint32_t frames = 0;
        while (!_isDestroyed && _framesRead < originalTotalFrames)
        {
            frames = std::min(framesToReadOnce, remainingFrames);
            if (_framesRead + frames > originalTotalFrames)
            {
                frames = originalTotalFrames - _framesRead;
            }
            framesRead = decoder->read(frames, _pcmData + _framesRead * bytesPerFrame);
            if (framesRead == 0)
                break;
            _framesRead += framesRead;
            remainingFrames -= framesRead;
        }

        if (_isDestroyed)
            return;

        if (_framesRead < originalTotalFrames)
        {
            memset(_pcmData + _framesRead * bytesPerFrame, 0x00, (totalFrames - _framesRead) * bytesPerFrame);
        }
        Logger::getInstance().info("pcm buffer was loaded successfully, total frames: %u, total read frames: %u, adjust frames: %u, remainingFrames: %u", totalFrames, _framesRead, adjustFrames, remainingFrames);

        _framesRead += adjustFrames;

        alBufferData(_alBufferId, _format, _pcmData, (ALsizei)dataSize, (ALsizei)sampleRate);

        _state = State::READY;
    }
    else//音频是大文件时
    {
        _queBufferFrames = static_cast<uint32_t>(sampleRate * QUEUEBUFFER_TIME_STEP);//
        if (_queBufferFrames == 0)
        {
            Logger::getInstance().error("%s: _queBufferFrames == 0, sampleRate: %u, QUEUEBUFFER_TIME_STEP: %f", __FUNCTION__, sampleRate, QUEUEBUFFER_TIME_STEP);
            return;
        }
        const uint32_t queBufferBytes = _queBufferFrames * bytesPerFrame;

        for (int index = 0; index < QUEUEBUFFER_NUM; ++index)
        {
            _queBuffers[index] = (char*)malloc(queBufferBytes);
            _queBufferSize[index] = queBufferBytes;

            decoder->readFixedFrames(_queBufferFrames, _queBuffers[index]);
        }

        _state = State::READY;
    }



    if (decoder != nullptr)
    {
        decoder->close();
    }

    AudioDecoderFactory::destroyDecoder(decoder);

    if (_state != State::READY)
    {
        _state = State::FAILED;
        if (_alBufferId != INVALID_AL_BUFFER_ID && alIsBuffer(_alBufferId))
        {
            Logger::getInstance().error("readDataTask failed, delete buffer: %u", _alBufferId);
            alDeleteBuffers(1, &_alBufferId);
            _alBufferId = INVALID_AL_BUFFER_ID;
        }
    }


    invokingPlayCallbacks();
    _isLoadingFinished = true;

}

void AudioData::invokingPlayCallbacks()
{
    std::lock_guard<std::mutex> lk(_playCallbackMutex);

    for (auto&& cb : _playCallbacks)
    {
        cb();
    }

    _playCallbacks.clear();
}
