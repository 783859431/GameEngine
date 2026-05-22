#include "OggDecoder.h"
#include "../utils/FileUtils.h"
bool OggDecoder::open(const char* path)
{
    std::string fullPath = FileUtils::GetInst()->GetFileFullPath(path);
    if (0 == ov_fopen(FileUtils::GetInst()->TransPath(fullPath).c_str(), &_vf))
    {
        // header
        vorbis_info* vi = ov_info(&_vf, -1);
        _sampleRate = static_cast<uint32_t>(vi->rate);
        _channelCount = vi->channels;
        _bytesPerFrame = vi->channels * sizeof(short);
        _totalFrames = static_cast<uint32_t>(ov_pcm_total(&_vf, -1));
        _isOpened = true;
        return true;
    }
    return false;
}

void OggDecoder::close()
{
    if (isOpened())
    {
        ov_clear(&_vf);
        _isOpened = false;
    }
}

uint32_t OggDecoder::read(uint32_t framesToRead, char* pcmBuf)
{
    int currentSection = 0;
    int bytesToRead = framesToRead * _bytesPerFrame;
    long bytesRead = ov_read(&_vf, pcmBuf, bytesToRead, 0, 2, 1, &currentSection);
    return static_cast<uint32_t>(bytesRead / _bytesPerFrame);
}

bool OggDecoder::seek(uint32_t frameOffset)
{
    return 0 == ov_pcm_seek(&_vf, frameOffset);
}

uint32_t OggDecoder::tell() const
{
    return static_cast<uint32_t>(ov_pcm_tell(const_cast<OggVorbis_File*>(&_vf)));
}

OggDecoder::OggDecoder()
{
}

OggDecoder::~OggDecoder()
{
    close();
}
