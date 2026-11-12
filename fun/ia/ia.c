#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include <random.h>
#include <limits.h>

#include "vector.h"


typedef struct HiddenNeuron_ {
    float b;
    Vector w;
    float in;
    float out;
} HiddenNeuron;



float get_random_float() {
    return (float)rand() / (float)FLOAT_MAX;

}

void init_hidden_neurons(Vector *ns, size_t n, siez_t cur_last_layer_size) {
    if (n == NULL)
        return false;
    for (size_t i = 0; i < n; i++) {
        HiddenNeuron hn;
        hn.b = get_random_float(); 
        vector_init(&hn.w, sizeof(float));
        for (size_t j = 0; j < cur_last_layer_size; j++) {
            vector_push_back(&hn.w, get_random_float());
        }

        vector_push_back(ns, hn);
    }
    return true;
}

void random_hidden_neurons(HiddenNeuron *n) {
    
}

typedef struct NeuralManager_ {
    Vector input_layer;
    Vector hidden_layers;
    Vector output_layer;

} NeuralManager;

void init_vec_values(Vector *v, const void *value, size_t n) {
    for (size_t i = 0; i < n; i++)
        vector_push_back(v, value);
}

int main() {
    printf("----------  INITIALISATION  ---------- \n");

    const size_t INPUT_SIZE = 1;
    const size_t HIDDEN_LAYER_SIZE = 2;
    const size_t HIDDEN_TOTAL_SIZE = 2;
    const size_t OUTPUT_SIZE = 1;

    NeuralManager nm;
    vector_init(&nm.input_layer, sizeof(float));
    vector_init(&nm.hidden_layers, sizeof(Vector));
    vector_init(&nm.output_layer, sizeof(float));
    float zero = 0.0;
    init_vec_values(&nm.input_layer, &zero, INPUT_SIZE);
    init_vec_values(&nm.hidden_layers, &zero, HIDDEN_TOTAL_SIZE);
    for (size_t i = 0; i < HIDDEN_TOTAL_SIZE; i++) {
        Vector current_hid_layer;
        vector_get(&nm.hidden_layers, i, &current_hid_layer);
        vector_init(&current_hid_layer, sizeof(HiddenNeuron));
        init_hidden_neurons(&current_hid_layer, &zero, HIDDEN_LAYER_SIZE);
        vector_set(&current_hid_layer, i, &current_hid_layer);
    }
    init_vec_values(&nm.output_layer, &zero, OUTPUT_SIZE);
    




    Vector numbers;
    vector_init(&numbers, 4);

    int a = 10, b = 20, c = 30;
    vector_push_back(&numbers, &a);
    vector_push_back(&numbers, &b);
    vector_push_back(&numbers, &c);

    vector_pop_back(&numbers);

    int value;
    vector_get(&numbers, 1, &value);

    vector_free(&numbers);

    printf("----------  FINI  ----------\n");

    return 0;
}