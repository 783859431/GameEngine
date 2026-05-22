#pragma once
#include <vorbis/vorbisfile.h>
#include "AudioDecoder.h"
class OggDecoder :
    public AudioDecoder
{
public:
    /**
     * @brief Opens an audio file specified by a file path.
     * @return true if succeed, otherwise false.
     */
    virtual bool open(const char* path) override;

    /**
     * @brief Closes opened audio file.
     * @note The method will also be automatically invoked in the destructor.
     */
    virtual void close() override;

    /**
     * @brief Reads audio frames of PCM format.
     * @param framesToRead The number of frames excepted to be read.
     * @param pcmBuf The buffer to hold the frames to be read, its size should be >= |framesToRead| * _bytesPerFrame.
     * @return The number of frames actually read, it's probably less than 'framesToRead'. Returns 0 means reach the end of file.
     */
    virtual uint32_t read(uint32_t framesToRead, char* pcmBuf) override;

    /**
     * @brief Sets frame offest to be read.
     * @param frameOffset The frame offest to be set.
     * @return true if succeed, otherwise false
     */
    virtual bool seek(uint32_t frameOffset) override;

    /**
     * @brief Tells the current frame offset.
     * @return The current frame offset.
     */
    virtual uint32_t tell() const override;

protected:
    OggDecoder();
    ~OggDecoder();

    OggVorbis_File _vf;
    friend class AudioDecoderFactory;
};

