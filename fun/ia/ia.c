#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

typedef struct Neuron_ {
    float value;
} Neuron;
#define NEURON_SIZE sizeof(Neuron)

typedef struct HiddenNeuron_ {
    float b;
    float *w;
    size_t w_size;
    float out;
    char act;
    float z;
    float delta;

} HiddenNeuron;
#define HIDDEN_NEURON_SIZE sizeof(HiddenNeuron)

float relu(const float x) {
    if (x < 0.0)
        return 0.0f;
    return x;
}

float relu_derivative(float x) {
    return x > 0.0f ? 1.0f : 0.0f;
}

float activation_derivative(float x, char a) {
    switch (a) {
        case 't': {
            float t = tanhf(x);
            return 1.0f - t * t;
        }
        case 'r':
            return relu_derivative(x);
        default:
            return 1.0f;
    }
}

float activation_function(const float x, const char a) {
    switch (a) {
        case 't':
           return tanh(x);
        case 'r':
            return relu(x);
        case 'l':
            return x;
       default:
            return x;
    }
}


float get_random_float() {
    return (float)rand() / RAND_MAX;
}

float get_random_float_10() {
    return get_random_float() * 10 - 5;
}

typedef struct HiddenLayer_ {
    HiddenNeuron *neurons;
    size_t hidden_size;

} HiddenLayer;

typedef struct NeuralNetwork_ {
    Neuron *input_layer;
    size_t input_size;
    HiddenLayer *hidden_layers;
    size_t layer_size;

} NeuralNetwork;

typedef struct NeuralNetworkParams_ {
    size_t input_size;
    size_t hidden_layer_size;
    size_t *hidden_neurons_size;
    char *activation_functions;

} NeuralNetworkParams;

NeuralNetwork newNeuralNetwork(NeuralNetworkParams *nnp) {
    NeuralNetwork nn;

    nn.input_layer = malloc(sizeof(Neuron) * nnp->input_size);
    nn.input_size = nnp->input_size;
    Neuron n = { 0.0 };
    for (int i = 0; i < nnp->input_size; i++)
        memcpy(&nn.input_layer[i], &n, NEURON_SIZE);

    nn.layer_size = nnp->hidden_layer_size;
    nn.hidden_layers = malloc(sizeof(HiddenLayer) * nn.layer_size);

    for (size_t i = 0; i  < nnp->hidden_layer_size; i++) {
        HiddenNeuron *hln = malloc(sizeof(HiddenNeuron) * nnp->hidden_neurons_size[i]);
        HiddenLayer hl = { hln , nnp->hidden_neurons_size[i] };
        nn.hidden_layers[i] = hl;
        for (size_t j = 0; j < nnp->hidden_neurons_size[i]; j++) {
            HiddenNeuron *n = &nn.hidden_layers[i].neurons[j];
            n->b = get_random_float_10();
            n->w_size = (i == 0) ? nn.input_size : nn.hidden_layers[i - 1].hidden_size;
            n->w = malloc(sizeof(float) * n->w_size);
            n->z = 0.0f;
            n->out = 0.0f;
            n->delta = 0.0f;
            n->act = nnp->activation_functions[i];

            for (size_t k = 0; k < n->w_size; k++)
                n->w[k] = get_random_float_10();
        }
    }

    return nn;
}

void destroyNeuralNetwork(NeuralNetwork *nn) {
    free(nn->input_layer);

    for (size_t i = 0; i < nn->layer_size; i++) {
        for (size_t j = 0; j < nn->hidden_layers[i].hidden_size; j++)
            free(nn->hidden_layers[i].neurons[j].w);

        free(nn->hidden_layers[i].neurons);
    }

    free(nn->hidden_layers);
}

void printNeuralNetwork(NeuralNetwork *nn) {
    printf("Input Layer : \n [");
    for (size_t i = 0; i < nn->input_size; i++) {
        printf("%f", nn->input_layer[i].value);
        if (i != nn->input_size - 1) printf(", ");
    }
    printf("]\n");
    printf("Hidden Layers : \n");
    for (size_t i = 0; i < nn->layer_size; i++) {
        printf("[");
        for (size_t j = 0; j < nn->hidden_layers[i].hidden_size; j++) {
            printf("(%f, %c, ", nn->hidden_layers[i].neurons[j].b, nn->hidden_layers[i].neurons[j].act);
            for (size_t k = 0; k < nn->hidden_layers[i].neurons[j].w_size; k++) {
                printf("(%f", nn->hidden_layers[i].neurons[j].w[k]);
                if (k != nn->hidden_layers[i].neurons->w_size - 1) printf(", ");
            }
            printf(")");
            if (j != nn->hidden_layers[i].hidden_size - 1) printf(", ");
        }
        printf("]\n");
    }
}

void testNeuralNetwork(NeuralNetwork *nn) {
    if (!nn->layer_size || !nn->input_size)
        return ;

    for (size_t j = 0; j < nn->hidden_layers[0].hidden_size; j++) {
        float sum = nn->hidden_layers[0].neurons[j].b;
        for (size_t k = 0; k < nn->hidden_layers[0].neurons[j].w_size; k++)
            sum += nn->hidden_layers[0].neurons[j].w[k] * nn->input_layer[k].value;
        nn->hidden_layers[0].neurons[j].z = sum;
        nn->hidden_layers[0].neurons[j].out = activation_function(sum, nn->hidden_layers[0].neurons[j].act);
    }
    for (size_t i = 1; i < nn->layer_size; i++) {
        for (size_t j = 0; j < nn->hidden_layers[i].hidden_size; j++) {
            float sum = nn->hidden_layers[i].neurons[j].b;
            for (size_t k = 0; k < nn->hidden_layers[i].neurons[j].w_size; k++)
                sum += nn->hidden_layers[i].neurons[j].w[k] * nn->hidden_layers[i - 1].neurons[k].out;
            nn->hidden_layers[i].neurons[j].z = sum;
            nn->hidden_layers[i].neurons[j].out = activation_function(sum, nn->hidden_layers[i].neurons[j].act);
        }
    }
}

typedef struct TrainingData_ {
    float **inputs;
    float **outputs;
    size_t count;

} TrainingData;

void backward(NeuralNetwork *nn, const float *target) {
    size_t last = nn->layer_size - 1;

    HiddenLayer *output = &nn->hidden_layers[last];
    for (size_t j = 0; j < output->hidden_size; j++) {
        HiddenNeuron *n = &output->neurons[j];
        n->delta = (n->out - target[j]) * activation_derivative(n->z, n->act);
    }

    //Propagation
    for (size_t l = last; l > 0; l--) {
        HiddenLayer *current = &nn->hidden_layers[l];
        HiddenLayer *previous = &nn->hidden_layers[l - 1];

        for (size_t j = 0; j < previous->hidden_size; j++) {
            float error = 0.0f;
            for (size_t k = 0; k < current->hidden_size; k++)
                error += current->neurons[k].w[j] * current->neurons[k].delta;
            HiddenNeuron *n = &previous->neurons[j];
            n->delta = error * activation_derivative(n->z, n->act);
        }
    }
}

void updateWeights(NeuralNetwork *nn, float learning_rate) {
    for (size_t l = 0; l < nn->layer_size; l++) {
        HiddenLayer *hl = &nn->hidden_layers[l];

        for (size_t j = 0; j < hl->hidden_size; j++) {
            HiddenNeuron *hn = &hl->neurons[j];
            hn->b -= learning_rate * hn->delta;

            for (size_t k = 0; k < hn->w_size; k++) {
                float previous_out;
                if (l == 0)
                    previous_out = nn->input_layer[k].value;
                else
                    previous_out = nn->hidden_layers[l - 1].neurons[k].out;

                hn->w[k] -= learning_rate * hn->delta * previous_out;
            }
        }
    }
}

void trainNeuralNetwork(NeuralNetwork *nn, TrainingData *td, size_t epochs, float learning_rate) {
    if (!nn->layer_size || !nn->input_size || !td->count)
        return;

    size_t last = nn->layer_size - 1;
    for (size_t epoch = 0; epoch < epochs; epoch++) {
        float total_loss = 0.0f;

        for (size_t sample = 0; sample < td->count; sample++) {

            for (size_t i = 0; i < nn->input_size; i++)
                nn->input_layer[i].value = td->inputs[sample][i];

            //forward prop
            testNeuralNetwork(nn);

            //loss
            HiddenLayer *output = &nn->hidden_layers[last];
            for (size_t j = 0; j < output->hidden_size; j++) {
                float error = output->neurons[j].out - td->outputs[sample][j];
                total_loss += 0.5f * error * error;
            }

            //backprop
            backward(nn, td->outputs[sample]);

            //grad descent
            updateWeights(nn, learning_rate);
        }
        total_loss /= td->count;

        if (epoch % 100 == 0) {
            printf("Epoch %zu | Loss: %f\n", epoch, total_loss);
        }
    }
}

int main() {
    srand((unsigned)time(NULL));

    size_t layers_size[3] = {2, 2, 1};
    char act_functions[3] = {'t', 't', 'l'};

    NeuralNetworkParams nnp = {
        1, 3, layers_size, act_functions
    };

    NeuralNetwork nn = newNeuralNetwork(&nnp);

    size_t count = 5;

    float **inputs = malloc(sizeof(float*) * count);
    float **outputs = malloc(sizeof(float*) * count);

    for (size_t i = 0; i < count; i++) {
        inputs[i] = malloc(sizeof(float));
        outputs[i] = malloc(sizeof(float));

        inputs[i][0] = (float)i;
        outputs[i][0] = inputs[i][0] * 2.0f + 2.0f;
    }

    TrainingData tr = { inputs, outputs, count };
    trainNeuralNetwork(&nn, &tr, 10000, 0.01f);


    for (float x = 0; x <= 4; x += 0.5f) {
        nn.input_layer[0].value = x;
        testNeuralNetwork(&nn);

        printf("x = %.2f | prediction = %.4f | expected = %.4f\n",
            x,
            nn.hidden_layers[2].neurons[0].out,
            2.0f * x + 2.0f
        );
    }

    for (size_t i = 0; i < count; i++) {
        free(inputs[i]);
        free(outputs[i]);
    }

    free(inputs);
    free(outputs);

    destroyNeuralNetwork(&nn);

    return 0;
}