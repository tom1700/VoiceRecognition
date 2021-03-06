#define NEURON_NO_ERROR (1)
#define PERCEPTRON_NO_ERROR (1)

typedef struct Neuron{
	int inputsAmount;
	double * inputWeights;
	double output;
	double error;
} Neuron;

typedef struct Perceptron{
	int inputSize;
	int hiddenSize;
	int outputSize;
	Neuron * inputLayer;
	Neuron * hiddenLayer;
	Neuron * outputLayer;
}Perceptron;

double sigmoidFunction(double t);
double sigmoidDerivative(double t);
double getError(double output, double properOutput);


int initNeuron(Neuron * neuron, int prevLayerSize);
int activateNeuron(Neuron * neuron, Neuron * prevLayer);
int destroyNeuron(Neuron * neuron);

int initPerceptron(Perceptron * perceptron, int inputSize, int hiddenSize, int outputSize);
int setPerceptronInputLayer(Perceptron * perceptron, double * input);
int activatePerceptron(Perceptron * perceptron);
int propagatePerceptronError(Perceptron * perceptron, double * properOutput, double learningRate);
int printPerceptronOutputLayer(Perceptron * perceptron);
int destroyPerceptron(Perceptron * perceptron);