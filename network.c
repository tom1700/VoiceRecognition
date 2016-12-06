#include <stdio.h>
#include <stdlib.h>
#include<string.h> 
#include <time.h>
#include <math.h>
#include <fcntl.h>
#include <dirent.h>

#define NO_NET_ERR (1)
#define NET_ERR (0)
#define MAX_LINE_WIDTH (1048576)

#define NUMPAT 4
#define NUMIN  2
#define NUMHID 2
#define NUMOUT 1

typedef struct Network{
    int      InputSize;
    int      HiddenSize;
    int      OutputSize;
    float   *Input;
    float   *Hidden;
    float   *Output;
    float   **WeightIH;
    float   **WeightHO;
} Network;

int initNet(Network * net, int inputSize, int hiddenSize, int outputSize){
    net->InputSize = inputSize;
    net->HiddenSize = hiddenSize;
    net->OutputSize = outputSize;
    if((net->Input = calloc(inputSize + 1, sizeof(float))) == NULL){ 
        perror("Input initialization");
        return NET_ERR;
    };
    if((net->Hidden = calloc(hiddenSize + 1, sizeof(float))) == NULL){ 
        perror("Hidden initialization");
        return NET_ERR;
    };
    if((net->Output = calloc(outputSize + 1, sizeof(float))) == NULL){ 
        perror("Output initialization");
        return NET_ERR;
    };
    if((net->WeightIH = malloc(sizeof(float*) * (hiddenSize + 1))) == NULL){ 
        perror("WeightIH initialization");
        return NET_ERR;
    };
    for(int i = 0; i <= hiddenSize; i++){
        if((net->WeightIH[i] = calloc(inputSize + 1, sizeof(float))) == NULL){ 
            perror("WeightIH[i] initialization");
            return NET_ERR;
        };
    }
    if((net->WeightHO = malloc(sizeof(float*) * (outputSize + 1))) == NULL){ 
        perror("WightHO initialization");
        return NET_ERR;
    };
    for(int i = 0; i <= outputSize; i++){
        if((net->WeightHO[i] = calloc(hiddenSize + 1 , sizeof(float))) == NULL){ 
            perror("WightHO[i] initialization");
            return NET_ERR; 
        };
    }
    net->Input[0] = net->Hidden[0] = net->Output[0] = 1;
    return NO_NET_ERR;
}

int deleteNet(Network * net){
    free(net->Input);
    free(net->Hidden);
    free(net->Output);
    for(int i = 0; i < net->HiddenSize + 1; i++){
        free(net->WeightIH[i]);
    }
    free(net->WeightIH);
    for(int i = 0; i < net->OutputSize + 1; i++){
        free(net->WeightHO[i]);
    }
    free(net->WeightHO);
    printf("     Delete succeeded\n");
    return NO_NET_ERR;
}

int drawWeights(Network * net){
    srand(time(NULL));
    for(int i=0 ; i < net->HiddenSize + 1; i++ ) {
        for(int j = 0 ; j < net->InputSize + 1; j++ ) {
            net->WeightIH[i][j] = ((float)rand() / (float)RAND_MAX) * 2.0 - 1.0;
        }
    }
    for(int i = 0 ; i < net->OutputSize + 1; i++ ) {
        for(int j = 0 ; j < net->HiddenSize + 1 ; j++ ) {
            net->WeightHO[i][j] = ((float)rand() / (float)RAND_MAX) * 2.0 - 1.0;
        }
    }
    return NO_NET_ERR;
}

int saveNetToFile(Network * net){
    FILE *f = fopen("network.txt", "w");
    if (f == NULL){
        printf("Error opening file!\n");
        exit(1);
    }
    fprintf(f, "InputSize: %d\n", net->InputSize);
    fprintf(f, "HiddenSize: %d\n", net->HiddenSize);
    fprintf(f, "OutputSize: %d\n", net->OutputSize);

    for(int i = 0; i < net->HiddenSize + 1; i++){
        for(int j = 0; j < net->InputSize + 1; j++){
            fprintf(f, "%.9g ", net->WeightIH[i][j]);
        }
        fprintf(f,"\n");
    }
    fprintf(f,"\n");

    for(int i = 0; i < net->OutputSize + 1; i++){
        for(int j = 0; j < net->HiddenSize + 1; j++){
            fprintf(f, "%.9g ", net->WeightHO[i][j]);
        }
        fprintf(f,"\n");   
    }
    fclose(f);
    return NO_NET_ERR;
}

int loadNetFromFile(Network * net){
    FILE *f = fopen("network.txt", "r");
    if (f == NULL){
        printf("Error opening file!\n");
        exit(1);
    }
    char lineBuff[MAX_LINE_WIDTH];
    int InputSize;
    int HiddenSize;
    int OutputSize;
    
    fgets(lineBuff, MAX_LINE_WIDTH, f);
    sscanf(lineBuff, "InputSize: %d\n", &InputSize);
    memset(lineBuff, 0, MAX_LINE_WIDTH);
    
    fgets(lineBuff, MAX_LINE_WIDTH, f);
    sscanf(lineBuff, "HiddenSize: %d\n", &HiddenSize);
    memset(lineBuff, 0, MAX_LINE_WIDTH);

    fgets(lineBuff, MAX_LINE_WIDTH, f);
    sscanf(lineBuff, "OutputSize: %d\n", &OutputSize);
    memset(lineBuff, 0, MAX_LINE_WIDTH);

    initNet(net, InputSize, HiddenSize, OutputSize);

    for(int i = 0; i < net->HiddenSize + 1; i++){
        fgets(lineBuff, MAX_LINE_WIDTH, f);
        char *delim = " ";
        char *token = NULL;
        int j = 0;
        for (token = strtok(lineBuff, delim); token != NULL; token = strtok(NULL, delim)){
            char *unconverted;
            net->WeightIH[i][j] = strtod(token, &unconverted);
            j++;
        }
        memset(lineBuff, 0, MAX_LINE_WIDTH);        
    }
    fgets(lineBuff, MAX_LINE_WIDTH, f);
    memset(lineBuff, 0, MAX_LINE_WIDTH);
    for(int i = 0; i < net->OutputSize + 1; i++){
        fgets(lineBuff, MAX_LINE_WIDTH, f);
        char *delim = " ";
        char *token = NULL;
        int j = 0;
        for (token = strtok(lineBuff, delim); token != NULL; token = strtok(NULL, delim)){
            char *unconverted;
            net->WeightHO[i][j] = strtod(token, &unconverted);
            j++;
        }
        memset(lineBuff, 0, MAX_LINE_WIDTH);   
    }
    fclose(f);
    return NO_NET_ERR;
}

int setInput(Network * net, float * input){
    for(int i = 1; i <= net->InputSize; i++){
        net->Input[i] = input[i-1];
    }
    return NO_NET_ERR;
}

int activate(Network * net){
    float Sum;
    for(int i = 1; i <= net->HiddenSize; i++) { 
        Sum = 0;
        for(int j = 0; j <= net->InputSize ; j++ ) {
            Sum += net->Input[j] * net->WeightIH[i][j] ;
        }
        net->Hidden[i] = 1.0/(1.0 + exp(-Sum));
    }
    for(int i = 1; i <= net->OutputSize; i++) { 
        Sum = 0;
        for(int j = 0; j <= net->HiddenSize; j++ ) {
            Sum += net->Hidden[j] * net->WeightHO[i][j];
        }
        net->Output[i] = 1.0/(1.0 + exp(-Sum));
    }
    return NO_NET_ERR;
}

int backpropagate(Network * net,
                  float   * targetOutput,
                  float   eta, 
                  float   alpha,    
                  float   * deltaO,
                  float   * deltaH,
                  float   * sumDOW,
                  float   ** deltaWeightIH,
                  float   ** deltaWeightHO){
    deltaO[0] = 0.0;
    deltaH[0] = 0.0;
    sumDOW[0] = 0.0;
    
    for(int i = 1; i <= net->OutputSize; i++){
        deltaO[i] = (targetOutput[i - 1] - net->Output[i]) * net->Output[i] * (1.0 - net->Output[i]);
    }
    for(int i = 1; i <= net->HiddenSize; i++) {
        sumDOW[i] = 0.0;
        for(int j = 1; j <= net->OutputSize; j++ ) {
            sumDOW[i] += net->WeightHO[j][i] * deltaO[j];
        }
        deltaH[i] = sumDOW[i] * net->Hidden[i] * (1.0 - net->Hidden[i]);
    }
    for(int i = 1; i <= net->HiddenSize; i++){
        deltaWeightIH[i][0] = eta * deltaH[i] + alpha * deltaWeightIH[i][0] ;
        net->WeightIH[i][0] += deltaWeightIH[i][0];
        for(int j = 1; j <= net->InputSize; j++){ 
            deltaWeightIH[i][j] = eta * net->Input[j] * deltaH[i] + alpha * deltaWeightIH[i][j];
            net->WeightIH[i][j] += deltaWeightIH[i][j];
        }
    }
    for(int i = 1; i <= net->OutputSize; i++){
        deltaWeightHO[i][0] = eta * deltaO[i] + alpha * deltaWeightHO[i][0] ;
        net->WeightHO[i][0] += deltaWeightHO[i][0] ;
        for(int j = 1; j <= net->HiddenSize; j++) {
            deltaWeightHO[i][j] = eta * net->Hidden[j] * deltaO[i] + alpha * deltaWeightHO[i][j] ;
            net->WeightHO[i][j] += deltaWeightHO[i][j] ;
        }
    }
    return NO_NET_ERR;
}

int teach(Network * net, float **inputs, float **outputs, int patternsAmount, int epochs, float eta, float alpha){
    float * deltaO = malloc((net->OutputSize + 1) * sizeof(float));
    float * deltaH = malloc((net->HiddenSize + 1) * sizeof(float));
    float * sumDOW = malloc((net->HiddenSize + 1) * sizeof(float));
    float ** deltaWeightIH = malloc(sizeof(float*) * (net->HiddenSize + 1));
    float ** deltaWeightHO = malloc(sizeof(float*) * (net->OutputSize + 1));
    for(int i = 0; i < net->HiddenSize + 1; i++){
        deltaWeightIH[i] = calloc(net->InputSize + 1, sizeof(float));
    }    
    for(int i = 0; i < net->OutputSize + 1; i++){
        deltaWeightHO[i] = calloc(net->HiddenSize + 1 , sizeof(float));
    }
    for(int i = 0; i < epochs; i++){
        for(int j = 0; j < patternsAmount; j++){
            setInput(net, inputs[j]);
            activate(net);
            backpropagate(net, outputs[j], eta, alpha, deltaO, deltaH, sumDOW, deltaWeightIH, deltaWeightHO);
        }
    }
    
    for(int i = 0; i < net->HiddenSize + 1; i++){
        free(deltaWeightIH[i]);
    } 
    for(int i = 0; i < net->OutputSize + 1; i++){
        free(deltaWeightHO[i]);
    }
    free(deltaO);
    free(deltaH);
    free(sumDOW);
    free(deltaWeightIH);
    free(deltaWeightHO);
    return NO_NET_ERR;
}

int loadTrainingDataFromDir(float      ***inputs, 
                            float      ***outputs, 
                            const char * dirName, 
                            int        * trainingPatterns, 
                            int        outputIndex){
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
            (*inputs) = realloc((*inputs), ((*trainingPatterns) + 1) * sizeof(float*));
            (*inputs) = realloc((*outputs), ((*trainingPatterns) + 1) * sizeof(float*));
            
            sprintf(pathname, "%s/%s", dirName, pDirent->d_name );
            if ((fp = fopen(pathname, "rb")) == NULL) {
                printf ("Cannot open file '%s'\n", pDirent->d_name);
                return 0;
            }
            memset(pathname, 0, 1024);
            if(((*inputs)[*trainingPatterns] = malloc(66150 * (sizeof(float)))) == NULL){
                perror("Loading data memory allocation");
                exit(1);
            }
            if(((*outputs)[*trainingPatterns] = malloc(4 * (sizeof(float)))) == NULL){
                perror("Loading data memory allocation");
                exit(1);
            }
            fread((void*)inputs[*trainingPatterns], sizeof(float), 66150, fp);
            ((*outputs)[*trainingPatterns])[0] = 0.0;
            ((*outputs)[*trainingPatterns])[1] = 0.0;
            ((*outputs)[*trainingPatterns])[2] = 0.0;
            ((*outputs)[*trainingPatterns])[3] = 0.0;
            ((*outputs)[*trainingPatterns])[outputIndex] = 1.0;
            fclose(fp);
            (*trainingPatterns)++;
        }
    }
    closedir(pDir);
    return 1;
}

int loadTrainingDataFromFiles(float ***inputs, float ***outputs, int *trainingPatterns){
    const char* dir1Name = "records/training/zapal";
    const char* dir2Name = "records/training/zgas";
    const char* dir3Name = "records/training/rozjasnij";
    const char* dir4Name = "records/training/przyciemnij";
    *trainingPatterns = 0;
    (*inputs) = malloc(sizeof(float));
    (*outputs) = malloc(sizeof(float));
            
    //zapal -       [1.0, 0.0, 0.0, 0.0]
    loadTrainingDataFromDir(inputs, outputs, dir1Name, trainingPatterns, 0);
    //zgas -        [0.0, 1.0, 0.0, 0.0]
    loadTrainingDataFromDir(inputs, outputs, dir2Name, trainingPatterns, 1);
    //rozjasnij -   [0.0, 0.0, 1.0, 0.0]
    loadTrainingDataFromDir(inputs, outputs, dir3Name, trainingPatterns, 2);
    //przyciemnij - [0.0, 0.0, 0.0, 1.0]
    loadTrainingDataFromDir(inputs, outputs, dir4Name, trainingPatterns, 3);

    return 1;
}
//-------------------------------------------------Tests------------------------------------------------------------
int initAndDeleteTest(){
    printf("InitAndDeleteTest:\n");
    Network net;
    static int InputSize = 50;
    static int HiddenSize = 100;
    static int OutputSize = 4;
    
    if(initNet(&net, InputSize, HiddenSize, OutputSize) == NET_ERR){
        perror("     -InitNet Failed");
        return 0;
    }
    if(InputSize != net.InputSize){
        printf("     -InitNet Failed: %d != %d\n", InputSize, net.InputSize);
        return 0;
    }
    if(HiddenSize != net.HiddenSize){
        printf("     -InitNet Failed: %d != %d\n", HiddenSize, net.HiddenSize);
        return 0;
    }
    if(OutputSize != net.OutputSize){
        printf("     -InitNet Failed: %d != %d\n", OutputSize, net.OutputSize);
        return 0;
    }
    printf("     -InitNet Succeded\n");

    if(deleteNet(&net) == NET_ERR){
        perror("     -DeleteNet Failed");
        return 0;
    }
    else{
        printf("     -DeleteNet Succeded\n");
    }
    
    return 1;
}

int randomWeightsTest(){
    printf("RandomWeightsTest:\n");
    Network net;
    static int InputSize = 50;
    static int HiddenSize = 100;
    static int OutputSize = 4;
    initNet(&net, InputSize, HiddenSize, OutputSize);
    if(drawWeights(&net) == NET_ERR){
        perror("Draw weights:");
        return 0;
    };
    for(int i = 0; i < net.HiddenSize + 1; i++){
        for(int j = 0; j < net.InputSize + 1; j++){
            if(net.WeightIH[i][j] > 1.0 || net.WeightIH[i][j] < -1.0){
                return 0;
            }
        }
    }
    for(int i=0; i < net.OutputSize + 1; i++){
        for(int j = 0; j < net.HiddenSize + 1; j++){
            if(net.WeightHO[i][j] > 1.0 || net.WeightHO[i][j] < -1.0){
                return 0;
            }
        }
    }
    deleteNet(&net);
    return 1;
}

int saveAndLoadFromFileTest(){
    printf("SaveAndLoadFromFileTest:\n");
    Network net;
    static int InputSize = 50;
    static int HiddenSize = 100;
    static int OutputSize = 4;
    initNet(&net, InputSize, HiddenSize, OutputSize);
    drawWeights(&net);
    saveNetToFile(&net);
    
    Network net2;
    loadNetFromFile(&net2);

    if(net.InputSize != net2.InputSize){
        printf("     -LoadNet Failed: %d != %d\n", net.InputSize, net2.InputSize);
        return 0;
    }
    if(net.HiddenSize != net2.HiddenSize){
        printf("     -LoadNet Failed: %d != %d\n", net.HiddenSize, net2.HiddenSize);
        return 0;
    }
    if(net.OutputSize != net2.OutputSize){
        printf("     -LoadNet Failed: %d != %d\n", net.OutputSize, net2.OutputSize);
        return 0;
    }
    for(int i = 0; i < net.HiddenSize + 1; i++){
        for(int j = 0; j < net.InputSize + 1; j++){
            if(floor(net.WeightIH[i][j] * 1000000) != floor(net2.WeightIH[i][j] * 1000000)){
                printf("     -LoadNet Failed: net.WeightIH[%d][%d] %f != %f\n", i, j, net.WeightIH[i][j], net2.WeightIH[i][j]);
                return 0;
            }
        }
    }
    for(int i = 0; i < net.OutputSize + 1; i++){
        for(int j = 0; j < net.HiddenSize + 1; j++){
            if(floor(net.WeightHO[i][j] * 1000000) != floor(net2.WeightHO[i][j] * 1000000)){
                printf("     -LoadNet Failed: net. WeightHO[%d][%d] %f != %f\n", i, j, net.WeightHO[i][j], net2.WeightHO[i][j]);
                return 0;
            }
        }   
    }
    deleteNet(&net);
    return 1;
}

int activateTest(){
    printf("ActivateTest:\n");
    Network net;
    static int InputSize = 10;
    static int HiddenSize = 100;
    static int OutputSize = 2;
    initNet(&net, InputSize, HiddenSize, OutputSize);
    drawWeights(&net);
    float input1 [] = { 0.5, 0.75, 0.7, -0.1, 0.9, -0.6, -0.5, -0.7, -0.45, 0.33 };
    float input2 [] = { 0.6, 0.55, 0.75, -0.11, 0.97, -0.56, -0.15, -0.17, -0.457, 0.233 };
    float output1[3];
    float output2[3];
    setInput(&net, input1);
    activate(&net);
    output1[1] = net.Output[1];
    output1[2] = net.Output[2];
    setInput(&net, input2);
    activate(&net);
    output2[1] = net.Output[1];
    output2[2] = net.Output[2];
    if(output1[1] == output2[1] && output1[2] == output2[2]){
        printf("     -Activate Failed: Same output1[1]:%f == output2[1]:%f, output1[2]:%f == output2[2]:%f\n",
                output1[1],
                output2[1],
                output1[2],
                output2[2]
              );
        return 0;
    }
    deleteNet(&net);
    return 1;
}

int setInputTest(){
    printf("SetInputTest:\n");
    Network net;
    static int InputSize = 10;
    static int HiddenSize = 2;
    static int OutputSize = 1;
    initNet(&net, InputSize, HiddenSize, OutputSize);
    drawWeights(&net);
    float inputs[10] = { 0.5, 0.75, 0.7, -0.1, 0.9, -0.6, -0.5, -0.7, -0.45, 0.33 };
    setInput(&net,inputs);
    for(int i = 1; i <= 10; i++){
        if(inputs[i-1] != net.Input[i]){
            printf("     -Set input Failed: i:%d %f != %f\n",i, inputs[i-1], net.Input[i]);
            return 0;
        }
    }
    printf("      -Set input Succeeded\n");
    deleteNet(&net);
    return 1;
}

int xorTest(){
    printf("XORTest:\n");
    Network net;
    static int epochs = 10000;
    static float eta = 0.2;
    static float alpha = 0.5;
    static int InputSize = 2;
    static int HiddenSize = 2;
    static int OutputSize = 1;
    initNet(&net, InputSize, HiddenSize, OutputSize);
    drawWeights(&net);
    float ** inputs = malloc(sizeof(float*) * 4);
    float ** outputs = malloc(sizeof(float*) * 4);
    for(int i = 0; i < 4; i++){
        inputs[i] = malloc(sizeof(float) * 2);
        outputs[i] = malloc(sizeof(float) * 2);
    }
    inputs[0][0] = 0.0; inputs[0][1] = 0.0;
    inputs[1][0] = 0.0; inputs[1][1] = 1.0;
    inputs[3][0] = 1.0; inputs[3][1] = 1.0;
    inputs[2][0] = 1.0; inputs[2][1] = 0.0;

    outputs[0][0] = 0.0;
    outputs[1][0] = 1.0;
    outputs[2][0] = 0.0;
    outputs[3][0] = 1.0;
    teach(&net, inputs, outputs, 4, epochs, eta, alpha);
    
    for(int j = 0; j < 4; j++){
        printf("     Input:[");
        setInput(&net, inputs[j]);
        for(int i = 0; i < net.InputSize; i++){
            printf("%f ", inputs[j][i]);
        }
        printf("]\n     Output:[");
        activate(&net);
        for(int i = 1; i <= net.OutputSize; i++){
            printf("%f ", net.Output[i]);
        }
        printf("] Target:[");
        for(int i = 0; i < net.OutputSize; i++){
            printf("%f ", outputs[j][i]);
        }
        printf("]\n");
    }
    
    deleteNet(&net);
    for(int i = 0; i < 4; i++){
        free(inputs[i]);
        free(outputs[i]);
    }
    free(inputs);
    free(outputs);
    return 1;
}

int andTest(){
    printf("ANDTest:\n");
    Network net;
    static int epochs = 10000;
    static float eta = 0.2;
    static float alpha = 0.5;
    static int InputSize = 2;
    static int HiddenSize = 2;
    static int OutputSize = 1;
    initNet(&net, InputSize, HiddenSize, OutputSize);
    drawWeights(&net);
    float ** inputs = malloc(sizeof(float*) * 4);
    float ** outputs = malloc(sizeof(float*) * 4);
    for(int i = 0; i < 4; i++){
        inputs[i] = malloc(sizeof(float) * 2);
        outputs[i] = malloc(sizeof(float) * 2);
    }
    inputs[0][0] = 0.0; inputs[0][1] = 0.0;
    inputs[1][0] = 0.0; inputs[1][1] = 1.0;
    inputs[3][0] = 1.0; inputs[3][1] = 1.0;
    inputs[2][0] = 1.0; inputs[2][1] = 0.0;

    outputs[0][0] = 0.0;
    outputs[1][0] = 0.0;
    outputs[2][0] = 1.0;
    outputs[3][0] = 0.0;
    teach(&net, inputs, outputs, 4, epochs, eta, alpha);
    
    for(int j = 0; j < 4; j++){
        printf("     Input:[");
        setInput(&net, inputs[j]);
        for(int i = 0; i < net.InputSize; i++){
            printf("%f ", inputs[j][i]);
        }
        printf("]\n     Output:[");
        activate(&net);
        for(int i = 1; i <= net.OutputSize; i++){
            printf("%f ", net.Output[i]);
        }
        printf("] Target:[");
        for(int i = 0; i < net.OutputSize; i++){
            printf("%f ", outputs[j][i]);
        }
        printf("]\n");
    }
    
    deleteNet(&net);
    for(int i = 0; i < 4; i++){
        free(inputs[i]);
        free(outputs[i]);
    }
    free(inputs);
    free(outputs);
    return 1;
}

int dirTest(){
    printf("DirTest:\n");
    float **inputs;
    float **outputs;
    int patternsAmount;
    if(loadTrainingDataFromFiles(&inputs, &outputs, &patternsAmount) == 0){
        return 0;
    }
    for(int i = 0; i < patternsAmount; i++){
        printf("Pattern:%d Output:[%f, %f, %f, %f]\n", i, outputs[i][0], outputs[i][1], outputs[i][2], outputs[i][3]);
    }
    for(int i = 0; i < patternsAmount; i++){
        free(inputs[i]);
        free(outputs[i]);
    }
    free(inputs);
    free(outputs);
    return 1;
}

int finalTest(const char * arg){
    static int epochs = 10000;
    static float eta = 0.1;
    static float alpha = 0.5;
    float **inputs;
    float **outputs;
    int patternsAmount;
    Network net;
    initNet(&net, 66150, 100, 4);
    if(strcmp(arg, "-l") == 0){
        loadNetFromFile(&net);
    }
    else{
        drawWeights(&net);
    }
    loadTrainingDataFromFiles(&inputs, &outputs, &patternsAmount);
    teach(&net, inputs, outputs, patternsAmount, epochs, eta, alpha);

    for(int i = 0; i < patternsAmount; i++){
        setInput(&net, inputs[i]);
        activate(&net);
        printf("Pattern:%d Outputs:[%f, %f, %f, %f] Target:[%f, %f, %f, %f]",
            i,
            net.Output[1],
            net.Output[2],
            net.Output[3],
            net.Output[4],
            outputs[i][0],
            outputs[i][1],
            outputs[i][2],
            outputs[i][3]);
    }
    saveNetToFile(&net);
    deleteNet(&net);
    for(int i = 0; i < patternsAmount; i++){
        free(inputs[i]);
        free(outputs[i]);
    }
    free(inputs);
    free(outputs);
    return 1;
}

int main(int argc, char ** argv) {
	/*if(initAndDeleteTest()){
        printf("Init and delete network test: SUCCESS\n\n");
    }
    else{
        printf("Init and delete network test: FAILED\n\n");
    }
	if(randomWeightsTest()){
        printf("Random weights test: SUCCESS\n\n");
    }
    else{
        printf("Random weights test: FAILED\n\n");
    }
	if(saveAndLoadFromFileTest()){
        printf("Save and load from file test: SUCCESS\n\n");
    }
    else{
        printf("Save and load from file test: FAILED\n\n");
    }
    if(activateTest()){
        printf("Activate test: SUCCESS\n\n");
    }
    else{
        printf("Activate test: FAILED\n\n");
    }
    if(setInputTest()){
        printf("Set input test: SUCCESS\n\n");
    }
    else{
        printf("Set input test: FAILED\n\n");
    }
    if(xorTest()){
        printf("XOR test: SUCCESS\n\n");
    }
    else{
        printf("XOR test: FAILED\n\n");
    }
    if(andTest()){
        printf("AND test: SUCCESS\n\n");
    }
    else{
        printf("AND test: FAILED\n\n");
    }
    if(dirTest()){
        printf("Dir test: SUCCESS\n\n");
    }
    else{
        printf("Dir test: FAILED\n\n");
    }*/
    if(argc > 2){
        finalTest(argv[1]);
    }
    else{
        finalTest("");
    }
    return 0;
}