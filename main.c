#include <stdio.h>
#include <stdlib.h>
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

int main(){/*
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
	};*/
	
	Perceptron perceptron;
	if(initPerceptron(&perceptron,2,1,1) != PERCEPTRON_NO_ERROR){
		printf("Init perceptron error\n");
		return 1;
	}
	double input1[2] = {1.0, 1.0};
	double input2[2] = {1.0, 0.0};
	double input3[2] = {0.0, 1.0};
	double input4[2] = {0.0, 0.0};
	double output1[1] = {1.0};
	double output2[1] = {0.0};
	double output3[1] = {0.0};
	double output4[1] = {1.0};


	for(int i=0; i<20; i++){
		setPerceptronInputLayer(&perceptron, input1);
		activatePerceptron(&perceptron);
		propagatePerceptronError(&perceptron, output1, 0.1);
		
		setPerceptronInputLayer(&perceptron, input2);
		activatePerceptron(&perceptron);
		propagatePerceptronError(&perceptron, output2, 0.1);

		setPerceptronInputLayer(&perceptron, input3);
		activatePerceptron(&perceptron);
		propagatePerceptronError(&perceptron, output3, 0.1);
		
		setPerceptronInputLayer(&perceptron, input4);
		activatePerceptron(&perceptron);
		propagatePerceptronError(&perceptron, output4, 0.1);
	}
	
	setPerceptronInputLayer(&perceptron, input1);
	activatePerceptron(&perceptron);
	printPerceptronOutputLayer(&perceptron);
	
	setPerceptronInputLayer(&perceptron, input2);
	activatePerceptron(&perceptron);
	printPerceptronOutputLayer(&perceptron);
	
	setPerceptronInputLayer(&perceptron, input3);
	activatePerceptron(&perceptron);
	printPerceptronOutputLayer(&perceptron);
	
	setPerceptronInputLayer(&perceptron, input4);
	activatePerceptron(&perceptron);
	printPerceptronOutputLayer(&perceptron);
	
	if(destroyPerceptron(&perceptron) != PERCEPTRON_NO_ERROR){
		printf("Destroy perceptron error\n");
		return 1;
	}
	return 0;
}