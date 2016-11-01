#include <stdio.h>
#include "portaudio.h"
#include "network.h"
#define NUM_SECONDS (30)

static int paTestCallback( const void *inputBuffer, void *outputBuffer,
                           unsigned long framesPerBuffer,
                           const PaStreamCallbackTimeInfo* timeInfo,
                           PaStreamCallbackFlags statusFlags,
                           void *userData ){
		char* data = (char*)inputBuffer;
		for (int i = 0; i < framesPerBuffer; i++){
			printf("%02X ", data[i]);
		}
		//printf("%d ",framesPerBuffer);
    return 0;
}

int main(){
	PaError error;
	if((error = Pa_Initialize()) != paNoError){
		printf("%s\n", Pa_GetErrorText(error));
		return 1;
	};
	
	PaStream * stream;
	
	if((error = Pa_OpenDefaultStream(&stream, 1, 0, paFloat32, 44100, 256, paTestCallback, NULL)) != paNoError){
		printf("%s\n", Pa_GetErrorText(error));
		return 1;
	}
	
	if((error = Pa_StartStream( stream )) != paNoError){
		printf("%s\n", Pa_GetErrorText(error));
		return 1;
	}
	
	Pa_Sleep(NUM_SECONDS * 1000);
	
	if((error = Pa_StopStream( stream )) != paNoError){
		printf("%s\n", Pa_GetErrorText(error));
		return 1;
	}
	
	if(error = (Pa_Terminate()) != paNoError){
		printf("%s\n", Pa_GetErrorText(error));
		return 1;
	};
	return 0;
	
}