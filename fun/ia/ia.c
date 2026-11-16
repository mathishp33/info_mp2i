#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include <limits.h>
#include <float.h>
#include <string.h>

typedef struct Neuron_ {
    float value;
} Neuron;
#define NEURON_SIZE sizeof(Neuron)

typedef struct HiddenNeuron_ {
    float b;
    float *w;
    float w_size;
    float in;
    float out;
    char act;
} HiddenNeuron;
#define HIDDEN_NEURON_SIZE sizeof(HiddenNeuron)


float get_random_float() {
    return (float)rand() / (float)FLT_MAX;
}

float get_random_float_10() {
    get_random_float() * 20 - 10;
}

typedef struct HiddenLayer_ {
    HiddenNeuron *neurons;
    size_t hidden_size;

} HiddenLayer;

typedef struct NeuralNetwork_ {
    Neuron *input_layer;
    size_t input_layer_size;
    HiddenLayer *hidden_layers;
    size_t hidden_layers_size;
    Neuron *output_layer;
    size_t output_layer_size;
} NeuralNetwork;

typedef struct NeuralNetworkParams_ {
    size_t input_size;
    size_t output_size;
    size_t hidden_layer_size;
    size_t *hidden_neurons_size;
    char *activation_functions;

} NeuralNetworkParams;

NeuralNetwork newNeuralNetwork(NeuralNetworkParams *nnp) {
    NeuralNetwork nn;

    nn.input_layer = malloc(sizeof(Neuron) * nnp->input_size);
    Neuron n = { 0.0 };
    for (int i = 0; i < nnp->input_size; i++)
        memcpy(&nn.input_layer[i], &n, NEURON_SIZE);

    nn.hidden_layers = malloc(sizeof(HiddenNeuron) * nnp->hidden_layer_size);

    for (size_t i = 0; i  < nnp->hidden_layer_size; i++) {
        HiddenNeuron *hln = malloc(sizeof(HiddenNeuron) * nnp->hidden_neurons_size[i]);
        HiddenLayer hl = { &hln , nnp->hidden_neurons_size[i] };
        for (size_t j = 0; j < nnp->hidden_neurons_size[i]; j++) {
            HiddenNeuron hn;
            nn.hidden_layers[i].neurons[j].b = get_random_float_10(); //ca crash ici car il faut initialiser les structs à l'interieur des arrays
            nn.hidden_layers[i].neurons[j].act = nnp->activation_functions[i]; //ca crash ici car il faut initialiser les structs à l'interieur des arrays
            printf("%d \n", j);
            if (i == 0)
                nn.hidden_layers[i].neurons[j].w_size = nnp->input_size;
            else
                nn.hidden_layers[i].neurons[j].w_size = nn.hidden_layers[i - 1].hidden_size;
            for (size_t k = 0; k < nn.hidden_layers[i].neurons[j].w_size; k++)
                nn.hidden_layers[i].neurons[j].w[k] = get_random_float_10();
        }
    }

    return nn;
}

void destroyNeuralNetwork(NeuralNetwork *nn) {
    free(nn->input_layer);
    free(nn->hidden_layers);
}

void printNeuralNetwork(NeuralNetwork *nn) {
    printf("Input Layer : \n [");
    for (size_t i = 0; i < nn->input_layer_size; i++) {
        printf("%f", nn->input_layer[i].value);
        if (i != nn->input_layer_size - 1) printf(", ");
    }
    printf("]\n");

}

int main() {
    printf("----------  INITIALISATION  ---------- \n");

    size_t layers_size[3] = { 2, 2, 1 };
    char act_functions[3] = { 't', 't', 't' };
    NeuralNetworkParams nnp = { 1, 3, layers_size, act_functions };

    NeuralNetwork nn = newNeuralNetwork(&nnp);
    printNeuralNetwork(&nn);

    
    destroyNeuralNetwork(&nn);

    printf("----------  FINI  ----------\n");

    return 0;
}