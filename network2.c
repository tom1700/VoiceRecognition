#include <stdio.h>
#include <stdlib.h>
#include<string.h> 
#include <time.h>
#include <math.h>
#include <fcntl.h>
#include <dirent.h>

#define NO_NET_ERR (1)
#define NET_ERR (0)
#define MAX_LINE_WIDTH (2048576)
#define INPUT_SIZE (8192)
#define HIDDEN_SIZE (22)
#define OUTPUT_SIZE (4)
#define ETA (0.1f)
#define ALPHA (0.9f)
#define TRAINING_PATTERNS (120)
#define TEST_PATTERNS (24)

#ifndef PI
	#define PI (3.14159265358979323846264338327950288)
#endif

typedef struct{
    float Re;
    float Im;
} complex;

// Training data arrays
char    lineBuff        [MAX_LINE_WIDTH];
float   trainingInputs  [TRAINING_PATTERNS][INPUT_SIZE];
float   trainingOutputs [TRAINING_PATTERNS][OUTPUT_SIZE];
float   testInputs      [TEST_PATTERNS][INPUT_SIZE];
float   testOutputs     [TEST_PATTERNS][OUTPUT_SIZE];
// Network arrays
float   Input           [INPUT_SIZE  + 1];
float   Hidden          [HIDDEN_SIZE + 1];
float   Output          [OUTPUT_SIZE + 1];
float   WeightIH        [HIDDEN_SIZE + 1][INPUT_SIZE  + 1];
float   WeightHO        [OUTPUT_SIZE + 1][HIDDEN_SIZE + 1];
//Backpropagation arrays
float   deltaH          [HIDDEN_SIZE + 1];
float   sumDOW			[HIDDEN_SIZE + 1];
float   deltaO          [OUTPUT_SIZE + 1];
float   deltaWeightIH   [HIDDEN_SIZE + 1][INPUT_SIZE + 1];
float   deltaWeightHO   [OUTPUT_SIZE + 1][INPUT_SIZE + 1];

complex v               [INPUT_SIZE];
complex tmp             [INPUT_SIZE];

//-----------------------------------------------Helper functions----------------------------------------------
void fft(complex *v, int n, complex *tmp){
  if(n > 1) {
    int k, m;    
    complex z, w, *vo, *ve;
    ve = tmp; vo = tmp + n/2;
    for(k = 0; k < n/2; k++) {
      ve[k] = v[2*k];
      vo[k] = v[2*k+1];
    }
    fft( ve, n/2, v );
    fft( vo, n/2, v );
    for(m = 0; m < n/2; m++) {
      w.Re = cos(2 * PI * m/(double)n);
      w.Im = -sin(2 * PI * m/(double)n);
      z.Re = w.Re * vo[m].Re - w.Im * vo[m].Im;
      z.Im = w.Re * vo[m].Im + w.Im * vo[m].Re;
      v[  m  ].Re = ve[m].Re + z.Re;
      v[  m  ].Im = ve[m].Im + z.Im;
      v[m+n/2].Re = ve[m].Re - z.Re;
      v[m+n/2].Im = ve[m].Im - z.Im;
    }
  }
}

int performFFT(float data[INPUT_SIZE]){
    for(int i = 0; i < INPUT_SIZE / 2; i++){
        v[i].Re = data[i];
        v[i].Im = 0.0;
    }
    fft(v, INPUT_SIZE / 2, tmp);
    for(int i = 0; i < INPUT_SIZE / 2; i++){
        data[2*i] = v[i].Re;
        data[2*i+1] = v[i].Im;
    }
    return 1;
}

//-----------------------------------------------Net functions------------------------------------------------
int initNet(){
	//Setting Bias
    Input[0] = Hidden[0] = Output[0] = 1.0;

	//Drawing weights
	srand(time(NULL));
    for(int i = 0 ; i < HIDDEN_SIZE + 1; i++ ) {
        for(int j = 0 ; j < INPUT_SIZE + 1; j++ ) {
            WeightIH[i][j] = (((float)rand() / (float)RAND_MAX) * 2.0 - 1.0) * 0.1;
        }
    }
    for(int i = 0 ; i < OUTPUT_SIZE + 1; i++ ) {
        for(int j = 0 ; j < HIDDEN_SIZE + 1 ; j++ ) {
            WeightHO[i][j] = (((float)rand() / (float)RAND_MAX) * 2.0 - 1.0) * 0.1;
        }
    }
    return NO_NET_ERR;
}

int saveNetToFile(const char * name){
    FILE *f = fopen(name, "w");
    if (f == NULL){
        printf("Error opening file!\n");
        exit(1);
    }
    for(int i = 0; i < HIDDEN_SIZE + 1; i++){
        for(int j = 0; j < INPUT_SIZE + 1; j++){
            fprintf(f, "%.9g ", WeightIH[i][j]);
        }
        fprintf(f,"\n");
    }
    fprintf(f,"\n");

    for(int i = 0; i < OUTPUT_SIZE + 1; i++){
        for(int j = 0; j < HIDDEN_SIZE + 1; j++){
            fprintf(f, "%.9g ", WeightHO[i][j]);
        }
        fprintf(f,"\n");   
    }
    fclose(f);
    return NO_NET_ERR;
}

int loadNetFromFile(const char * name){
    FILE *f = fopen(name, "r");
    if (f == NULL){
        printf("Error opening file!\n");
        exit(1);
    }
    for(int i = 0; i < HIDDEN_SIZE + 1; i++){
        fgets(lineBuff, MAX_LINE_WIDTH, f);
        char *delim = " ";
        char *token = NULL;
        int j = 0;
        for (token = strtok(lineBuff, delim); token != NULL; token = strtok(NULL, delim)){
            char *unconverted;
            WeightIH[i][j] = strtod(token, &unconverted);
            j++;
        }
        memset(lineBuff, 0, MAX_LINE_WIDTH);        
    }
    fgets(lineBuff, MAX_LINE_WIDTH, f);
    memset(lineBuff, 0, MAX_LINE_WIDTH);
    for(int i = 0; i < OUTPUT_SIZE + 1; i++){
        fgets(lineBuff, MAX_LINE_WIDTH, f);
        char *delim = " ";
        char *token = NULL;
        int j = 0;
        for (token = strtok(lineBuff, delim); token != NULL; token = strtok(NULL, delim)){
            char *unconverted;
            WeightHO[i][j] = strtod(token, &unconverted);
            j++;
        }
        memset(lineBuff, 0, MAX_LINE_WIDTH);   
    }
    fclose(f);
    return NO_NET_ERR;
}

int setInput(float newInput[INPUT_SIZE]){
    for(int i = 1; i < INPUT_SIZE + 1; i++){
        Input[i] = newInput[i-1];
    }
    return NO_NET_ERR;
}

int activate(){
    float Sum;
    for(int i = 1; i <= HIDDEN_SIZE; i++) { 
        Sum = 0;
        for(int j = 0; j <= INPUT_SIZE ; j++ ) {
            Sum += Input[j] * WeightIH[i][j] ;
        }
        Hidden[i] = 1.0/(1.0 + exp(-Sum));
    }
    for(int i = 1; i <= OUTPUT_SIZE; i++) { 
        Sum = 0;
        for(int j = 0; j <= HIDDEN_SIZE; j++ ) {
            Sum += Hidden[j] * WeightHO[i][j];
        }
        Output[i] = 1.0/(1.0 + exp(-Sum));
    }
    return NO_NET_ERR;
}

int backpropagate(float targetOutput[OUTPUT_SIZE]){
    deltaO[0] = 0.0;
    deltaH[0] = 0.0;
    sumDOW[0] = 0.0;
    
    for(int i = 1; i <= OUTPUT_SIZE; i++){
        deltaO[i] = (targetOutput[i - 1] - Output[i]) * Output[i] * (1.0 - Output[i]);
    }
    for(int i = 1; i <= HIDDEN_SIZE; i++) {
        sumDOW[i] = 0.0;
        for(int j = 1; j <= OUTPUT_SIZE; j++ ) {
            sumDOW[i] += WeightHO[j][i] * deltaO[j];
        }
        deltaH[i] = sumDOW[i] * Hidden[i] * (1.0 - Hidden[i]);
    }
    for(int i = 1; i <= HIDDEN_SIZE; i++){
        deltaWeightIH[i][0] = ETA * deltaH[i] + ALPHA * deltaWeightIH[i][0] ;
        WeightIH[i][0] += deltaWeightIH[i][0];
        for(int j = 1; j <= INPUT_SIZE; j++){ 
            deltaWeightIH[i][j] = ETA * Input[j] * deltaH[i] + ALPHA * deltaWeightIH[i][j];
            WeightIH[i][j] += deltaWeightIH[i][j];
        }
    }
    for(int i = 1; i <= OUTPUT_SIZE; i++){
        deltaWeightHO[i][0] = ETA * deltaO[i] + ALPHA * deltaWeightHO[i][0] ;
        WeightHO[i][0] += deltaWeightHO[i][0] ;
        for(int j = 1; j <= HIDDEN_SIZE; j++) {
            deltaWeightHO[i][j] = ETA * Hidden[j] * deltaO[i] + ALPHA * deltaWeightHO[i][j] ;
            WeightHO[i][j] += deltaWeightHO[i][j] ;
        }
    }
    return NO_NET_ERR;
}

int teach(int patternsAmount, int epochs){
    for(int i = 0; i < OUTPUT_SIZE; i++){
		for(int j = 0; j < HIDDEN_SIZE; j++){
			deltaWeightHO[i][j] = 0.0;
		}
	}
	for(int i = 0; i < HIDDEN_SIZE; i++){
		for(int j = 0; j < INPUT_SIZE; j++){
			deltaWeightIH[i][j] = 0.0;
		}
	}
    for(int i = 0; i < epochs; i++){
        if(i % 100 != 0){
            for(int j = 0; j < patternsAmount / 4; j++){
                for(int k = 0; k < 4; k++){
                    setInput(trainingInputs[k * (patternsAmount / 4) + j]);
                    activate();
                    backpropagate(trainingOutputs[k * (patternsAmount / 4) + j]);
                }
            }
        }
        else{
            printf("Iteration: %d",i);
            float error = 0.0;
            for(int j = 0; j < patternsAmount / 4; j++){
                for(int k = 0; k < 4; k++){
                    setInput(trainingInputs[k * (patternsAmount / 4) + j]);
                    activate();
                    error += fabs(trainingOutputs[k * (patternsAmount / 4) + j][0] - Output[1]);
                    error += fabs(trainingOutputs[k * (patternsAmount / 4) + j][1] - Output[2]);
                    error += fabs(trainingOutputs[k * (patternsAmount / 4) + j][2] - Output[3]);
                    error += fabs(trainingOutputs[k * (patternsAmount / 4) + j][3] - Output[4]);
                    backpropagate(trainingOutputs[k * (patternsAmount / 4) + j]);
                }
            }
            printf(" Error: %f\n", error / patternsAmount);
        }
    }
    return NO_NET_ERR;
}

int loadTrainingDataFromDir(const char * dirName, int * trainingPatterns, int outputIndex){
    char pathname[1024];
    DIR *pDir;
    FILE *fp;
    int len;
    struct dirent *pDirent;    
    if ((pDir = opendir(dirName)) == NULL) {
        printf ("Cannot open directory '%s'\n", dirName);
        return 0;
    }
    while ((pDirent = readdir(pDir)) != NULL) {
        if(strcmp(pDirent->d_name,".") != 0 && strcmp(pDirent->d_name,"..") != 0){
            sprintf(pathname, "%s/%s", dirName, pDirent->d_name );
            if ((fp = fopen(pathname, "rb")) == NULL) {
                printf ("Cannot open file '%s'\n", pDirent->d_name);
                return 0;
            }
            memset(pathname, 0, 1024);
            fread((void*)trainingInputs[*trainingPatterns], sizeof(float), INPUT_SIZE / 2, fp);
            performFFT(trainingInputs[*trainingPatterns]);
            trainingOutputs[*trainingPatterns][0] = 0.0;
            trainingOutputs[*trainingPatterns][1] = 0.0;
            trainingOutputs[*trainingPatterns][2] = 0.0;
            trainingOutputs[*trainingPatterns][3] = 0.0;
            trainingOutputs[*trainingPatterns][outputIndex] = 1.0;
            fclose(fp);
            (*trainingPatterns)++;
        }
    }
    closedir(pDir);
    return 1;
}

int loadTestDataFromDir(const char * dirName, int * testPatterns, int outputIndex){
    char pathname[1024];
    DIR *pDir;
    FILE *fp;
    int len;
    struct dirent *pDirent;    
    if ((pDir = opendir(dirName)) == NULL) {
        printf ("Cannot open directory '%s'\n", dirName);
        return 0;
    }
    while ((pDirent = readdir(pDir)) != NULL) {
        if(strcmp(pDirent->d_name,".") != 0 && strcmp(pDirent->d_name,"..") != 0){
            sprintf(pathname, "%s/%s", dirName, pDirent->d_name );
            if ((fp = fopen(pathname, "rb")) == NULL) {
                printf ("Cannot open file '%s'\n", pDirent->d_name);
                return 0;
            }
            memset(pathname, 0, 1024);
            fread((void*)testInputs[*testPatterns], sizeof(float), INPUT_SIZE / 2, fp);
            performFFT(testInputs[*testPatterns]);
            testOutputs[*testPatterns][0] = 0.0;
            testOutputs[*testPatterns][1] = 0.0;
            testOutputs[*testPatterns][2] = 0.0;
            testOutputs[*testPatterns][3] = 0.0;
            testOutputs[*testPatterns][outputIndex] = 1.0;
            fclose(fp);
            (*testPatterns)++;
        }
    }
    closedir(pDir);
    return 1;
}

int loadTrainingDataFromFiles(int *trainingPatterns){
    const char* dir1Name = "records/training/zapal";
    const char* dir2Name = "records/training/zgas";
    const char* dir3Name = "records/training/rozjasnij";
    const char* dir4Name = "records/training/przyciemnij";
    *trainingPatterns = 0;
    //zapal -       [1.0, 0.0, 0.0, 0.0]
    loadTrainingDataFromDir(dir1Name, trainingPatterns, 0);
    //zgas -        [0.0, 1.0, 0.0, 0.0]
    loadTrainingDataFromDir(dir2Name, trainingPatterns, 1);
    //rozjasnij -   [0.0, 0.0, 1.0, 0.0]
    loadTrainingDataFromDir(dir3Name, trainingPatterns, 2);
    //przyciemnij - [0.0, 0.0, 0.0, 1.0]
    loadTrainingDataFromDir(dir4Name, trainingPatterns, 3);

    return 1;
}

int loadTestDataFromFiles(int *testPatterns){
    const char* dir1Name = "records/test/zapal";
    const char* dir2Name = "records/test/zgas";
    const char* dir3Name = "records/test/rozjasnij";
    const char* dir4Name = "records/test/przyciemnij";
    *testPatterns = 0;
    //zapal -       [1.0, 0.0, 0.0, 0.0]
    loadTestDataFromDir(dir1Name, testPatterns, 0);
    //zgas -        [0.0, 1.0, 0.0, 0.0]
    loadTestDataFromDir(dir2Name, testPatterns, 1);
    //rozjasnij -   [0.0, 0.0, 1.0, 0.0]
    loadTestDataFromDir(dir3Name, testPatterns, 2);
    //przyciemnij - [0.0, 0.0, 0.0, 1.0]
    loadTestDataFromDir(dir4Name, testPatterns, 3);
    return 1;
}

int maxIndex(float array[], int size, int startIndex){
    float max = array[startIndex];
    int maxIndex = startIndex;
    for(int i = startIndex + 1; i < size; i++){
        if(array[i] > max){
            maxIndex = i;
            max = array[i];
        }
    }
    return maxIndex;
}

int train(int epochs, const char * name, const char * load){
    int trainingPatternsAmount;
    int testPatternsAmount;
    if(strcmp(load, "-load") == 0){
        printf("Loading net from file\n");
        loadNetFromFile(name);
    }
    else{
		initNet();
    }

    loadTrainingDataFromFiles(&trainingPatternsAmount);
    teach(trainingPatternsAmount, epochs);
    loadTestDataFromFiles(&testPatternsAmount);

    float error = 0.0;
    int proper = 0;
    for(int i = 0; i < testPatternsAmount; i++){
        setInput(testInputs[i]);
        activate();
        error += fabs(testOutputs[i][0] - Output[1]);
        error += fabs(testOutputs[i][1] - Output[2]);
        error += fabs(testOutputs[i][2] - Output[3]);
        error += fabs(testOutputs[i][3] - Output[4]);
        int maxTestIndex = maxIndex(testOutputs[i], 4, 0);
        int maxOutputIndex = maxIndex(Output, 5, 1);
        if(maxTestIndex + 1 == maxOutputIndex){
            printf("Proper pattern: %d\n",i);
            proper++;
        }/*
        printf("Pattern:%d Outputs:[%f, %f, %f, %f] Target:[%f, %f, %f, %f]\n",
            i,
            Output[1],
            Output[2],
            Output[3],
            Output[4],
            testOutputs[i][0],
            testOutputs[i][1],
            testOutputs[i][2],
            testOutputs[i][3]);*/
    }
    printf("Proper: %d Total: %d\n", proper, testPatternsAmount);
    printf("Error %f", error / testPatternsAmount);
    saveNetToFile(name);
    return 1;
}

int main(int argc, char ** argv) {
    if(argc > 3){
        train(atoi(argv[1]), argv[2], argv[3]);
    }
    else if(argc > 2){
        train(atoi(argv[1]), argv[2], "");
    }
    else{
        train(atoi(argv[1]), "network.txt", "");
    }
    return 0;
}