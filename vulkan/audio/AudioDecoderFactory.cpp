#include "AudioDecoderFactory.h"
#include "../utils/FileUtils.h"
#include <string>
#include "Mp3Decoder.h"
#include "OggDecoder.h"
AudioDecoder* AudioDecoderFactory::createDecoder(const char* path)
{
    std::string suffix = FileUtils::GetInst()->GetFileExtension(path);
    if (suffix == ".ogg")
    {
        return new (std::nothrow) OggDecoder();
    }
    else if (suffix == ".mp3")
    {
        return new (std::nothrow) Mp3Decoder();
    }

    return nullptr;
}

void AudioDecoderFactory::destroyDecoder(AudioDecoder* decoder)
{
    delete decoder;

}
