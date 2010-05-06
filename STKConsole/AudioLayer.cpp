

#include "AudioLayer.h"
#include "urSound.h"
#include <math.h>
double angle=0;
RtAudio dac;
StkFrames frames;

int tick( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
		 double streamTime, RtAudioStreamStatus status, void *userData )
{
	register StkFloat *samples = (StkFloat *) outputBuffer;

	for ( unsigned int i=0; i<frames.size(); i++ ) {
		*samples++ = urs_PullActiveDacSingleTickSinks();
	}

	return 0;
}

void initializeAudioLayer() {
	RtAudio::StreamParameters parameters;
	parameters.deviceId = dac.getDefaultOutputDevice();
	parameters.nChannels = 1;
	RtAudioFormat format = ( sizeof(StkFloat) == 8 ) ? RTAUDIO_FLOAT64 : RTAUDIO_FLOAT32;
	unsigned int bufferFrames = RT_BUFFER_SIZE;
	frames.resize( bufferFrames, 1);
	try {
		dac.openStream( &parameters, NULL, format, (unsigned int)Stk::sampleRate(), &bufferFrames, &tick, NULL );
		dac.startStream();
	}
	catch ( RtError &error ) {
		error.printMessage();
	}
}

void playAudioLayer() {
	dac.startStream();
}

void stopAudioLayer() {
	dac.stopStream();
}

void cleanupAudioLayer() {
	dac.closeStream();
}

void* LoadAudioFileData(const char *filename, UInt32 *outDataSize, UInt32*	outSampleRate)
{
	FileWvIn input;

	char path[MAX_PATH];
	// Try to load the soundfile.
	try {
		sprintf(path,"..\\Sounds\\%s",filename);
		input.openFile( path );
	}
	catch ( StkError & ) {
		return NULL;
	}

	double rate = 1.0;
	rate = input.getFileRate() / Stk::sampleRate();
	input.setRate( rate );

	input.ignoreSampleRateChange();

	// Find out how many channels we have.
	int channels = input.channelsOut();
	*outDataSize=input.getSize();
	*outSampleRate=input.getFileRate();
	SInt16 *data=new SInt16[input.getSize()];
	for(unsigned int i=0;i<input.getSize();i++) {
		data[i]=(int)(input.tick()*32768.0);
	}

	input.closeFile();


	return data;
}
