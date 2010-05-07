
#ifndef __RIOAUDIOUNITLAYER_H__
#define __RIOAUDIOUNITLAYER_H__

#include "types.h"
#include "FileWvIn.h"
#include "RtAudio.h"
using namespace stk;


void initializeAudioLayer();
void playAudioLayer();
void stopAudioLayer();
void cleanupAudioLayer();

void* LoadAudioFileData(const char* filename, UInt32 *outDataSize, UInt32* outSampleRate);

#endif /* __RIOAUDIOUNITLAYER_H__ */
