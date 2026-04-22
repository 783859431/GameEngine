#pragma once
#include "AudioDecoder.h"
class AudioDecoderFactory
{
public:
	static AudioDecoder*createDecoder(const char* path);
	static void destroyDecoder(AudioDecoder* decoder);
};

