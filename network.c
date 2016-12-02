#include <stdio.h>
#include <stdlib.h>
#include<string.h> 
#include <time.h>
#include <math.h>
#include <fcntl.h>

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
    
    for(int j = 0; j < patternsAmount; j++){
        printf("Input:[");
        setInput(net, inputs[j]);
        for(int i = 0; i < net->InputSize; i++){
            printf("%f ", inputs[j][i]);
        }
        printf("]\nOutput:[");
        activate(net);
        for(int i = 1; i <= net->OutputSize; i++){
            printf("%f ", net->Output[i]);
        }
        printf("] Proper output:[");
        for(int i = 0; i < net->OutputSize; i++){
            printf("%f ", outputs[j][i]);
        }
        printf("]\n");
    }
    saveNetToFile(net);
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


/*
int network(){
    int    i, j, k, p, np, op, ranpat[NUMPAT+1], epoch;
    int    NumPattern = NUMPAT, NumInput = NUMIN, NumHidden = NUMHID, NumOutput = NUMOUT;
    float Input[NUMPAT+1][NUMIN+1] = { 0, 0, 0,  0, 0, 0,  0, 1, 0,  0, 0, 1,  0, 1, 1 };
    float Target[NUMPAT+1][NUMOUT+1] = { 0, 0,  0, 0,  0, 1,  0, 1,  0, 0 };
    float SumH[NUMPAT+1][NUMHID+1], WeightIH[NUMIN+1][NUMHID+1], Hidden[NUMPAT+1][NUMHID+1];
    float SumO[NUMPAT+1][NUMOUT+1], WeightHO[NUMHID+1][NUMOUT+1], Output[NUMPAT+1][NUMOUT+1];
    float DeltaO[NUMOUT+1], SumDOW[NUMHID+1], DeltaH[NUMHID+1];
    float DeltaWeightIH[NUMIN+1][NUMHID+1], DeltaWeightHO[NUMHID+1][NUMOUT+1];
    float Error, eta = 0.5, alpha = 0.9, smallwt = 0.5;
  
    for( j = 1 ; j <= NumHidden ; j++ ) {    // initialize WeightIH and DeltaWeightIH
        for( i = 0 ; i <= NumInput ; i++ ) { 
            DeltaWeightIH[i][j] = 0.0 ;
            WeightIH[i][j] = 2.0 * ( rando() - 0.5 ) * smallwt ;
        }
    }
    for( k = 1 ; k <= NumOutput ; k ++ ) {    // initialize WeightHO and DeltaWeightHO
        for( j = 0 ; j <= NumHidden ; j++ ) {
            DeltaWeightHO[j][k] = 0.0 ;              
            WeightHO[j][k] = 2.0 * ( rando() - 0.5 ) * smallwt ;
        }
    }
     
    for( epoch = 0 ; epoch < 100000 ; epoch++) {    // iterate weight updates 
        for( p = 1 ; p <= NumPattern ; p++ ) {    // randomize order of individuals 
            ranpat[p] = p ;
        }
        for( p = 1 ; p <= NumPattern ; p++) {
            np = p + rando() * ( NumPattern + 1 - p ) ;
            op = ranpat[p] ; ranpat[p] = ranpat[np] ; ranpat[np] = op ;
        }
        Error = 0.0 ;
        for( np = 1 ; np <= NumPattern ; np++ ) {    // repeat for all the training patterns 
            p = ranpat[np];
            for( j = 1 ; j <= NumHidden ; j++ ) {    // compute hidden unit activations 
                SumH[p][j] = WeightIH[0][j] ;
                for( i = 1 ; i <= NumInput ; i++ ) {
                    SumH[p][j] += Input[p][i] * WeightIH[i][j] ;
                }
                Hidden[p][j] = 1.0/(1.0 + exp(-SumH[p][j])) ;
            }
            for( k = 1 ; k <= NumOutput ; k++ ) {    // compute output unit activations and errors 
                SumO[p][k] = WeightHO[0][k] ;
                for( j = 1 ; j <= NumHidden ; j++ ) {
                    SumO[p][k] += Hidden[p][j] * WeightHO[j][k] ;
                }
                Output[p][k] = 1.0/(1.0 + exp(-SumO[p][k])) ;   // Sigmoidal Outputs
                //Output[p][k] = SumO[p][k];      Linear Outputs
                Error += 0.5 * (Target[p][k] - Output[p][k]) * (Target[p][k] - Output[p][k]) ;   // SSE 
                //Error -= ( Target[p][k] * log( Output[p][k] ) + ( 1.0 - Target[p][k] ) * log( 1.0 - Output[p][k] ) ) ;    Cross-Entropy Error
                DeltaO[k] = (Target[p][k] - Output[p][k]) * Output[p][k] * (1.0 - Output[p][k]) ;   /* Sigmoidal Outputs, SSE
                //DeltaO[k] = Target[p][k] - Output[p][k];     Sigmoidal Outputs, Cross-Entropy Error
                //DeltaO[k] = Target[p][k] - Output[p][k];     Linear Outputs, SSE
            }
            for( j = 1 ; j <= NumHidden ; j++ ) {    // 'back-propagate' errors to hidden layer
                SumDOW[j] = 0.0 ;
                for( k = 1 ; k <= NumOutput ; k++ ) {
                    SumDOW[j] += WeightHO[j][k] * DeltaO[k] ;
                }
                DeltaH[j] = SumDOW[j] * Hidden[p][j] * (1.0 - Hidden[p][j]) ;
            }
            for( j = 1 ; j <= NumHidden ; j++ ) {     /* update weights WeightIH
                DeltaWeightIH[0][j] = eta * DeltaH[j] + alpha * DeltaWeightIH[0][j] ;
                WeightIH[0][j] += DeltaWeightIH[0][j] ;
                for( i = 1 ; i <= NumInput ; i++ ) { 
                    DeltaWeightIH[i][j] = eta * Input[p][i] * DeltaH[j] + alpha * DeltaWeightIH[i][j];
                    WeightIH[i][j] += DeltaWeightIH[i][j] ;
                }
            }
            for( k = 1 ; k <= NumOutput ; k ++ ) {    // update weights WeightHO
                DeltaWeightHO[0][k] = eta * DeltaO[k] + alpha * DeltaWeightHO[0][k] ;
                WeightHO[0][k] += DeltaWeightHO[0][k] ;
                for( j = 1 ; j <= NumHidden ; j++ ) {
                    DeltaWeightHO[j][k] = eta * Hidden[p][j] * DeltaO[k] + alpha * DeltaWeightHO[j][k] ;
                    WeightHO[j][k] += DeltaWeightHO[j][k] ;
                }
            }
        }
        if( epoch%100 == 0 ) fprintf(stdout, "\nEpoch %-5d :   Error = %f", epoch, Error) ;
        if( Error < 0.0004 ) break ;  // stop learning when 'near enough'
    }
    
    fprintf(stdout, "\n\nNETWORK DATA - EPOCH %d\n\nPat\t", epoch) ;   // print network outputs 
    for( i = 1 ; i <= NumInput ; i++ ) {
        fprintf(stdout, "Input%-4d\t", i) ;
    }
    for( k = 1 ; k <= NumOutput ; k++ ) {
        fprintf(stdout, "Target%-4d\tOutput%-4d\t", k, k) ;
    }
    for( p = 1 ; p <= NumPattern ; p++ ) {        
    fprintf(stdout, "\n%d\t", p) ;
        for( i = 1 ; i <= NumInput ; i++ ) {
            fprintf(stdout, "%f\t", Input[p][i]) ;
        }
        for( k = 1 ; k <= NumOutput ; k++ ) {
            fprintf(stdout, "%f\t%f\t", Target[p][k], Output[p][k]) ;
        }
    }
    fprintf(stdout, "\n\nGoodbye!\n\n") ;
    return 1 ;
}
*/
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

int xorTest(int epochs, float eta, float alpha){
    printf("XORTest:\n");
    Network net;
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
    deleteNet(&net);
    for(int i = 0; i < 4; i++){
        free(inputs[i]);
        free(outputs[i]);
    }
    free(inputs);
    free(outputs);
    return 1;
}

int main(int argc, char ** argv) {
	if(initAndDeleteTest()){
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
    if(xorTest(atoi(argv[1]), atof(argv[2]), atof(argv[3]))){
        printf("XOR test: SUCCESS\n\n");
    }
    else{
        printf("XOR test: FAILED\n\n");
    }
    return 0;
}