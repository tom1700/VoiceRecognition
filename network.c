#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include "network.h"

int initNeuron(Neuron * neuron, int prevLayerSize){
	srand(time(NULL));

	neuron->inputsAmount = prevLayerSize;
	neuron->inputWeights = malloc(sizeof(float) * neuron->inputsAmount);
	neuron->output = 0.0;

	for(int i = 0; i < neuron->inputsAmount; i++){
		neuron->inputWeights[i] = pow(-1.0,rand()%2) * (double)rand() / RAND_MAX;
	}
	
	return NEURON_NO_ERROR;

}

double SigmoidFunction(double t){
	return 1/(1 + exp(-1 * t));
}

int activateNeuron(Neuron * neuron, Neuron * prevLayer){
	double output = 0.0;
	
	for(int i = 0; i < neuron->inputsAmount; i++){
		output += neuron->inputWeights[i] * prevLayer[i].output;
	}
	
	neuron->output = SigmoidFunction(output);
	
	return NEURON_NO_ERROR;
}

int destroyNeuron(Neuron * neuron){
	free(neuron->inputWeights);
	free(neuron);
	
	return NEURON_NO_ERROR;
}


int initPerceptron(Perceptron * perceptron, int inputSize, int hiddenSize, int outputSize){
	perceptron->inputSize = inputSize;
	perceptron->hiddenSize = hiddenSize;
	perceptron->outputSize = outputSize;
	
	perceptron->inputLayer = malloc(sizeof(Neuron) * perceptron->inputSize);
	perceptron->hiddenLayer = malloc(sizeof(Neuron) * perceptron->hiddenSize);
	perceptron->outputLayer = malloc(sizeof(Neuron) * perceptron->outputSize);

	for(int i = 0; i < perceptron->inputSize; i++){
		initNeuron(&perceptron->inputLayer[i], 0);
	}
	for(int i = 0; i < perceptron->hiddenSize; i++){
		initNeuron(&perceptron->hiddenLayer[i], perceptron->inputSize);
	}
	for(int i = 0; i < perceptron->outputSize; i++){
		initNeuron(&perceptron->outputLayer[i], perceptron->hiddenSize);
	}

	return PERCEPTRON_NO_ERROR;
}

int setPerceptronInputLayer(Perceptron * perceptron, double * input){
	for(int i = 0; i < perceptron->inputSize; i++){
		perceptron->inputLayer[i].output = input[i];
	}

	return PERCEPTRON_NO_ERROR;
}

int activatePerceptron(Perceptron * perceptron){
	return PERCEPTRON_NO_ERROR;
}

int printPerceptronOutputLayer(Perceptron * perceptron){
	for(int i = 0; i < perceptron->outputSize; i++){
		printf("%d ",perceptron->outputLayer[i].output);
	}
	printf("\n");

	return PERCEPTRON_NO_ERROR;
}

int destroyPerceptron(Perceptron * perceptron){
	for(int i = 0; i < perceptron->inputSize; i++){
		destroyNeuron(&perceptron->inputLayer[i]);
	}
	for(int i = 0; i < perceptron->hiddenSize; i++){
		destroyNeuron(&perceptron->hiddenLayer[i]);
	}
	for(int i = 0; i < perceptron->outputSize; i++){
		destroyNeuron(&perceptron->outputLayer[i]);
	}	
	free(perceptron->inputLayer);
	free(perceptron->hiddenLayer);
	free(perceptron->outputLayer);
	free(perceptron);
	return PERCEPTRON_NO_ERROR;
}