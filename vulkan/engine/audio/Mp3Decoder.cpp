#include "Mp3Decoder.h"
#include <mpg123.h>
#include <string>
#include "../logger/Logger.h"
#include "../utils/FileUtils.h"
static bool __mp3Inited = false;

bool Mp3Decoder::lazyInit()
{
    bool ret = true;
    if (!__mp3Inited)
    {
        int error = mpg123_init();
        if (error == MPG123_OK)
        {
            __mp3Inited = true;
        }
        else
        {
          
            Logger::getInstance().error("Basic setup goes wrong: %s", mpg123_plain_strerror(error));
            ret = false;
        }
    }
    return ret;
}

void Mp3Decoder::destroy()
{
    if (__mp3Inited)
    {
        mpg123_exit();
        __mp3Inited = false;
    }
}

Mp3Decoder::Mp3Decoder()
    : _mpg123handle(nullptr)
{
    lazyInit();
}

Mp3Decoder::~Mp3Decoder()
{
    close();
}

bool Mp3Decoder::open(const char* path)
{
    std::string fullPath = FileUtils::GetInst()->GetFileFullPath(path);

    long rate = 0;
    int error = MPG123_OK;
    int mp3Encoding = 0;
    int channel = 0;
    do
    {
        _mpg123handle = mpg123_new(nullptr, &error);
        if (nullptr == _mpg123handle)
        {
            Logger::getInstance().error("Basic setup goes wrong: %s", mpg123_plain_strerror(error));
            break;
        }

        if (mpg123_open(_mpg123handle, FileUtils::GetInst()->TransPath(fullPath).c_str()) != MPG123_OK
            || mpg123_getformat(_mpg123handle, &rate, &channel, &mp3Encoding) != MPG123_OK)
        {
            Logger::getInstance().error("Trouble with mpg123: %s\n", mpg123_strerror(_mpg123handle));
            break;
        }

        _channelCount = channel;// 左声道 右声道。。。
        _sampleRate = rate;   

        if (mp3Encoding == MPG123_ENC_SIGNED_16)
        {
            _bytesPerFrame = 2 * _channelCount;
        }
        else if (mp3Encoding == MPG123_ENC_FLOAT_32)
        {
            _bytesPerFrame = 4 * _channelCount;
        }
        else
        {
             Logger::getInstance().error("Bad encoding: 0x%x!\n", mp3Encoding);
            break;
        }

        /* Ensure that this output format will not change (it could, when we allow it). */
        mpg123_format_none(_mpg123handle);
        mpg123_format(_mpg123handle, rate, channel, mp3Encoding);
        /* Ensure that we can get accurate length by call mpg123_length */
        mpg123_scan(_mpg123handle);

        _totalFrames = mpg123_length(_mpg123handle); //

        _isOpened = true;
        return true;
    } while (false);

    if (_mpg123handle != nullptr)
    {
        mpg123_close(_mpg123handle);
        mpg123_delete(_mpg123handle);
        _mpg123handle = nullptr;
    }
    return false;
}

void Mp3Decoder::close()
{
    if (isOpened())
    {
        if (_mpg123handle != nullptr)
        {
            mpg123_close(_mpg123handle);
            mpg123_delete(_mpg123handle);
            _mpg123handle = nullptr;
        }
        _isOpened = false;
    }
}

uint32_t Mp3Decoder::read(uint32_t framesToRead, char* pcmBuf)
{
    int bytesToRead = framesToRead * _bytesPerFrame;
    size_t bytesRead = 0;
    int err = mpg123_read(_mpg123handle, (unsigned char*)pcmBuf, bytesToRead, &bytesRead);
    if (err == MPG123_ERR)
    {
        Logger::getInstance().error("Trouble with mpg123: %s\n", mpg123_strerror(_mpg123handle));
        return 0;
    }

    return static_cast<uint32_t>(bytesRead / _bytesPerFrame);
}

bool Mp3Decoder::seek(uint32_t frameOffset)
{
    off_t offset = mpg123_seek(_mpg123handle, frameOffset, SEEK_SET);
    //ALOGD("mpg123_seek return: %d", (int)offset);
    if (offset >= 0 && offset == frameOffset)
    {
        return true;
    }
    return false;
}

uint32_t Mp3Decoder::tell() const
{
    return static_cast<uint32_t>(mpg123_tell(_mpg123handle));
}
