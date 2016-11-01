#define NEURON_NO_ERROR (1)
#define NEURON_ERROR (0)

typedef struct Neuron{
	int inputsAmount;
	double * inputWeights;

} Neuron;

typedef struct Perceptron{

}Perceptron;

int initNeuron(Neuron * neuron, int prevLayerSize);
int calcNeuronOutput(Neuron * neuron, double * input, double * result);
int destroyNeuron(Neuron * neuron);