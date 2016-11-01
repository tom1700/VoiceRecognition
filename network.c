#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include "network.h"

double sigmoidFunction(double t){
	return 1/(1 + exp(-1 * t));
}

double getError(double output, double properOutput){
	return pow(output - properOutput, 2);
}


int initNeuron(Neuron * neuron, int prevLayerSize){
	srand(time(NULL));
	
	neuron->inputsAmount = prevLayerSize;
	neuron->inputWeights = malloc(sizeof(double) * (neuron->inputsAmount + 1));
	neuron->output = 0.0;

	for(int i = 0; i <= neuron->inputsAmount; i++){
		neuron->inputWeights[i] = pow(-1.0,rand()%2) * (double)rand() / RAND_MAX;
	}
	
	return NEURON_NO_ERROR;

}

int activateNeuron(Neuron * neuron, Neuron * prevLayer){
	double output = neuron->inputWeights[0];
	
	for(int i = 0; i < neuron->inputsAmount; i++){
		output += neuron->inputWeights[i+1] * prevLayer[i].output;
	}
	
	neuron->output = sigmoidFunction(output);
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
	for(int i = 0; i < perceptron->hiddenSize; i++){
		activateNeuron(&perceptron->hiddenLayer[i], perceptron->inputLayer);
	}
	for(int i = 0; i < perceptron->outputSize; i++){
		activateNeuron(&perceptron->outputLayer[i], perceptron->hiddenLayer);
	}
	return PERCEPTRON_NO_ERROR;
}

int propagatePerceptronError(Perceptron * perceptron, double * properOutput, double learningRate){
	for (int i = 0; i < perceptron->outputSize; i++){
		perceptron->outputLayer[i].error = (properOutput[i] - perceptron->outputLayer[i].output) * perceptron->outputLayer[i].output * (1.0 - perceptron->outputLayer[i].output);
	}
	for (int i = 0; i < perceptron->hiddenSize; i++){
		perceptron->hiddenLayer[i].error = 0.0;
		for(int j = 0; j < perceptron->outputSize; j++){
			perceptron->hiddenLayer[i].error += perceptron->outputLayer[j].error * perceptron->outputLayer[j].inputWeights[i+1];
		}
	}
	for (int i = 0; i < perceptron->hiddenSize; i++){
		perceptron->hiddenLayer[i].inputWeights[0] += (0.5 * perceptron->hiddenLayer[i].error + learningRate * perceptron->hiddenLayer[i].inputWeights[0]);
		for(int j = 0; j < perceptron->hiddenLayer[i].inputsAmount; j++){
			perceptron->hiddenLayer[i].inputWeights[j+1] += (0.5 * perceptron->hiddenLayer[i].error * perceptron->inputLayer[j].output + learningRate * perceptron->hiddenLayer[i].inputWeights[j+1]);
		}
	}
	for (int i = 0; i < perceptron->outputSize; i++){
		perceptron->outputLayer[i].inputWeights[0] += (0.5 * perceptron->outputLayer[i].error + learningRate * perceptron->outputLayer[i].inputWeights[0]);
		for(int j = 1; j < perceptron->outputLayer[i].inputsAmount; j++){
			perceptron->outputLayer[i].inputWeights[j+1] += (0.5 * perceptron->outputLayer[i].error * perceptron->hiddenLayer[j].output + learningRate * perceptron->outputLayer[i].inputWeights[j+1]);
		}
	}
	return PERCEPTRON_NO_ERROR;
}

int printPerceptronOutputLayer(Perceptron * perceptron){
	for(int i = 0; i < perceptron->outputSize; i++){
		printf("%f ",perceptron->outputLayer[i].output);
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