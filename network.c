#include <stdlib.h>
#include <time.h>
#include "network.h"

int initNeuron(Neuron * neuron, int prevLayerSize){
	srand(time(NULL));
	neuron->inputsAmount = prevLayerSize;
	neuron->inputWeights = malloc(sizeof(float) * neuron->inputsAmount);
	for(int i = 0; i < neuron->inputsAmount; i++){
		neuron->inputWeights[i] = (double)rand() / RAND_MAX;
	}
	return NEURON_NO_ERROR;
}


int calcNeuronOutput(Neuron * neuron, double * input, double * result){
	double output = 0.0;
	for(int i = 0; i < neuron->inputsAmount; i++){
		output += neuron->inputWeights[i] * input[i];
	}
	*result = output;
	return NEURON_NO_ERROR;
}


int destroyNeuron(Neuron * neuron){
	free(neuron->inputWeights);
	free(neuron);
	return NEURON_NO_ERROR;
}