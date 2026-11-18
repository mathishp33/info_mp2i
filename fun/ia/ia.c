/*
 * Idiot
 * Architecture :
 *   texte -> tokenizer -> Bag of Words -> MLP -> Softmax -> classe de reponse
 *
 * Ce programme apprend a associer une question a UN mot parmi les reponses
 * presentes dans le jeu d'entrainement. C'est drole parce que je l'ai fait moi meme
 * et que je peux décider des choses qu'il apprends.
 *
 * Utilisation :
 *   ./ia          -> charge tinylm.bin s'il existe, sinon entraine et sauvegarde
 *   ./ia --train  -> force un nouvel entrainement puis remplace tinylm.bin
 *   ./ia --load   -> charge tinylm.bin et echoue s'il est absent/invalide
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#include <stdint.h>

#define INPUT_BUFFER_SIZE 2048
#define CONFIDENCE_THRESHOLD 0.30f
#define DEFAULT_EPOCHS 5000
#define DEFAULT_LEARNING_RATE 0.001f
#define DEFAULT_BATCH_SIZE 4


static char *string_duplicate(const char *s) {
    size_t len = strlen(s);
    char *copy = malloc(len + 1);

    if (copy != NULL)
        memcpy(copy, s, len + 1);

    return copy;
}

static float random_uniform(float min_value, float max_value) {
    float r = (float)rand() / (float)RAND_MAX;
    return min_value + r * (max_value - min_value);
}

static bool is_token_separator(unsigned char c) {
    if (c >= 128)
        return false; // Conserve les octets UTF-8 non ASCII non converti

    return isspace(c) != 0 || ispunct(c) != 0;
}

static char *normalize_text(const char *text) {
    size_t len = strlen(text);
    char *out = malloc(2 * len + 1);
    if (out == NULL)
        return NULL;

    size_t j = 0;

    for (size_t i = 0; i < len;) {
        unsigned char c = (unsigned char)text[i];

        if (c < 0x80) {
            if (c >= 'A' && c <= 'Z')
                c = (unsigned char)(c - 'A' + 'a');
            out[j++] = (char)c;
            i++;
            continue;
        }

        if (c == 0xC3 && i + 1 < len) {
            unsigned char next = (unsigned char)text[i + 1];
            if (next >= 0x80 && next <= 0x9E && next != 0x97)
                next = (unsigned char)(next + 0x20);

            char replacement = '\0';
            switch (next) {
                case 0xA0: case 0xA1: case 0xA2: case 0xA3: case 0xA4: case 0xA5: replacement = 'a'; break;
                case 0xA6: out[j++] = 'a'; replacement = 'e'; break; /* ae */
                case 0xA7: replacement = 'c'; break;
                case 0xA8: case 0xA9: case 0xAA: case 0xAB: replacement = 'e'; break;
                case 0xAC: case 0xAD: case 0xAE: case 0xAF: replacement = 'i'; break;
                case 0xB0: replacement = 'd'; break;
                case 0xB1: replacement = 'n'; break;
                case 0xB2: case 0xB3: case 0xB4: case 0xB5: case 0xB6: case 0xB8: replacement = 'o'; break;
                case 0xB9: case 0xBA: case 0xBB: case 0xBC: replacement = 'u'; break;
                case 0xBD: case 0xBF: replacement = 'y'; break;
                case 0xBE: out[j++] = 't'; replacement = 'h'; break; /* thorn */
                case 0x9F: replacement = 's'; break;
                default: break;
            }

            if (replacement != '\0') {
                out[j++] = replacement;
                i += 2;
                continue;
            }
        }

        // Œ/œ -> oe
        if (c == 0xC5 && i + 1 < len) {
            unsigned char next = (unsigned char)text[i + 1];
            if (next == 0x92 || next == 0x93) {
                out[j++] = 'o';
                out[j++] = 'e';
                i += 2;
                continue;
            }
        }

        // char inconnu -> copie octet
        out[j++] = (char)c;
        i++;
    }

    out[j] = '\0';
    return out;
}


typedef struct TokenList_ {
    char **items;
    size_t count;
    size_t capacity;

} TokenList;

static void destroy_token_list(TokenList *tokens) {
    if (tokens == NULL)
        return;

    for (size_t i = 0; i < tokens->count; i++)
        free(tokens->items[i]);

    free(tokens->items);
    tokens->items = NULL;
    tokens->count = 0;
    tokens->capacity = 0;
}

static bool token_list_push(TokenList *tokens, const char *start, size_t len) {
    if (len == 0)
        return true;

    if (tokens->count == tokens->capacity) {
        size_t new_capacity = tokens->capacity == 0 ? 8 : tokens->capacity * 2;
        char **new_items = realloc(tokens->items, new_capacity * sizeof(*new_items));

        if (new_items == NULL)
            return false;

        tokens->items = new_items;
        tokens->capacity = new_capacity;
    }

    char *word = malloc(len + 1);
    if (word == NULL)
        return false;

    memcpy(word, start, len);
    word[len] = '\0';
    tokens->items[tokens->count++] = word;
    return true;
}

static bool tokenize(const char *text, TokenList *tokens) {
    if (text == NULL || tokens == NULL)
        return false;

    tokens->items = NULL;
    tokens->count = 0;
    tokens->capacity = 0;

    size_t len = 0;
    char *normalized = normalize_text(text);
    if (normalized == NULL)
        return false;
    len = strlen(normalized);

    size_t start = 0;
    bool inside_token = false;

    for (size_t i = 0; i <= len; i++) {
        unsigned char c = (unsigned char)normalized[i];
        bool separator = normalized[i] == '\0' || is_token_separator(c);

        if (!separator && !inside_token) {
            start = i;
            inside_token = true;
        }

        if (separator && inside_token) {
            if (!token_list_push(tokens, normalized + start, i - start)) {
                free(normalized);
                destroy_token_list(tokens);
                return false;
            }
            inside_token = false;
        }
    }

    free(normalized);
    return true;
}


typedef struct Vocabulary_ {
    char **words;
    size_t count;
    size_t capacity;

} Vocabulary;

static void destroy_vocabulary(Vocabulary *vocab) {
    if (vocab == NULL)
        return;

    for (size_t i = 0; i < vocab->count; i++)
        free(vocab->words[i]);

    free(vocab->words);
    vocab->words = NULL;
    vocab->count = 0;
    vocab->capacity = 0;
}

static size_t vocabulary_find(const Vocabulary *vocab, const char *word) {
    for (size_t i = 0; i < vocab->count; i++)
        if (strcmp(vocab->words[i], word) == 0)
            return i;

    return SIZE_MAX;
}

static bool vocabulary_add(Vocabulary *vocab, const char *word, size_t *id) {
    size_t existing = vocabulary_find(vocab, word);
    if (existing != SIZE_MAX) {
        if (id != NULL)
            *id = existing;
        return true;
    }

    if (vocab->count == vocab->capacity) {
        size_t new_capacity = vocab->capacity == 0 ? 16 : vocab->capacity * 2;
        char **new_words = (char **)realloc(
            vocab->words, new_capacity * sizeof(*new_words));

        if (new_words == NULL)
            return false;

        vocab->words = new_words;
        vocab->capacity = new_capacity;
    }

    char *copy = string_duplicate(word);
    if (copy == NULL)
        return false;

    size_t new_id = vocab->count;
    vocab->words[vocab->count++] = copy;

    if (id != NULL)
        *id = new_id;

    return true;
}


typedef struct QAExample_ {
    const char *question;
    const char *answer;

} QAExample;

typedef struct TrainingData_ {
    float *inputs; // count x input_size
    size_t *targets; // ID de la reponse
    size_t count;
    size_t input_size;

} TrainingData;

static void destroy_training_data(TrainingData *data) {
    if (data == NULL)
        return;

    free(data->inputs);
    free(data->targets);
    data->inputs = NULL;
    data->targets = NULL;
    data->count = 0;
    data->input_size = 0;
}

// mot est present ? 1 : 0
static bool vectorize_text(const char *text, const Vocabulary *input_vocab, float *vector, size_t vector_size,
    size_t *recognized_tokens) {
    if (text == NULL || input_vocab == NULL || vector == NULL || vector_size != input_vocab->count)
        return false;

    memset(vector, 0, vector_size * sizeof(*vector));

    TokenList tokens = { 0 };
    if (!tokenize(text, &tokens))
        return false;

    size_t recognized = 0;

    for (size_t i = 0; i < tokens.count; i++) {
        size_t id = vocabulary_find(input_vocab, tokens.items[i]);

        if (id != SIZE_MAX) {
            if (vector[id] == 0.0f) {
                vector[id] = 1.0f;
                recognized++;
            }
        }
    }

    destroy_token_list(&tokens);

    if (recognized_tokens != NULL)
        *recognized_tokens = recognized;

    return true;
}

static bool build_training_data(const QAExample *examples, size_t example_count, Vocabulary *input_vocab,
    Vocabulary *answer_vocab, TrainingData *data) {
    if (examples == NULL || example_count == 0 || input_vocab == NULL || answer_vocab == NULL || data == NULL)
        return false;

    memset(input_vocab, 0, sizeof(*input_vocab));
    memset(answer_vocab, 0, sizeof(*answer_vocab));
    memset(data, 0, sizeof(*data));

    for (size_t i = 0; i < example_count; i++) {
        if (examples[i].question == NULL || examples[i].answer == NULL)
            goto failure;

        TokenList tokens = {0};
        if (!tokenize(examples[i].question, &tokens))
            goto failure;

        for (size_t j = 0; j < tokens.count; ++j) {
            if (!vocabulary_add(input_vocab, tokens.items[j], NULL)) {
                destroy_token_list(&tokens);
                goto failure;
            }
        }
        destroy_token_list(&tokens);

        if (!vocabulary_add(answer_vocab, examples[i].answer, NULL))
            goto failure;
    }

    if (input_vocab->count == 0 || answer_vocab->count == 0)
        goto failure;

    if (example_count > SIZE_MAX / input_vocab->count)
        goto failure;

    size_t input_values_count = example_count * input_vocab->count;

    data->inputs = (float *)calloc(input_values_count, sizeof(*data->inputs));
    data->targets = (size_t *)malloc(example_count * sizeof(*data->targets));

    if (data->inputs == NULL || data->targets == NULL)
        goto failure;

    data->count = example_count;
    data->input_size = input_vocab->count;

    for (size_t i = 0; i < example_count; i++) {
        float *row = data->inputs + i * data->input_size;
        size_t recognized = 0;

        if (!vectorize_text(examples[i].question, input_vocab, row,
                            data->input_size, &recognized) || recognized == 0)
            goto failure;

        data->targets[i] = vocabulary_find(answer_vocab, examples[i].answer);
        if (data->targets[i] == SIZE_MAX)
            goto failure;
    }

    return true;

failure:
    destroy_training_data(data);
    destroy_vocabulary(input_vocab);
    destroy_vocabulary(answer_vocab);
    return false;
}


typedef struct Neuron_ {
    float value;

} Neuron;

typedef struct HiddenNeuron_ {
    float b;
    float *w;
    float *grad_w;
    size_t w_size;

    float z;
    float out;
    float delta; // d(Loss)/d(z)
    float grad_b;

    char act;
} HiddenNeuron;

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
    const size_t *hidden_neurons_size;
    const char *activation_functions;

} NeuralNetworkParams;

static float relu(float x) {
    return x > 0.0f ? x : 0.0f;
}

static float relu_derivative(float x) {
    return x > 0.0f ? 1.0f : 0.0f;
}

static float activation_function(float x, char activation) {
    switch (activation) {
        case 't': return tanhf(x);
        case 'r': return relu(x);
        case 'l':
        default:  return x;
    }
}

static float activation_derivative(float x, char activation) {
    switch (activation) {
        case 't': {
            float t = tanhf(x);
            return 1.0f - t * t;
        }
        case 'r': return relu_derivative(x);
        case 'l':
        default:  return 1.0f;
    }
}

static void destroy_neural_network(NeuralNetwork *nn) {
    if (nn == NULL)
        return;

    free(nn->input_layer);

    if (nn->hidden_layers != NULL) {
        for (size_t i = 0; i < nn->layer_size; i++) {
            if (nn->hidden_layers[i].neurons != NULL) {
                for (size_t j = 0; j < nn->hidden_layers[i].hidden_size; ++j) {
                    free(nn->hidden_layers[i].neurons[j].w);
                    free(nn->hidden_layers[i].neurons[j].grad_w);
                }
            }
            free(nn->hidden_layers[i].neurons);
        }
    }

    free(nn->hidden_layers);
    memset(nn, 0, sizeof(*nn));
}

static bool create_neural_network(
    NeuralNetwork *nn,
    const NeuralNetworkParams *params) {
    if (nn == NULL || params == NULL || params->input_size == 0 ||
        params->hidden_layer_size == 0 || params->hidden_neurons_size == NULL ||
        params->activation_functions == NULL)
        return false;

    memset(nn, 0, sizeof(*nn));

    nn->input_size = params->input_size;
    nn->layer_size = params->hidden_layer_size;

    nn->input_layer = (Neuron *)calloc(nn->input_size, sizeof(*nn->input_layer));
    nn->hidden_layers = (HiddenLayer *)calloc(nn->layer_size, sizeof(*nn->hidden_layers));

    if (nn->input_layer == NULL || nn->hidden_layers == NULL)
        goto failure;

    for (size_t l = 0; l < nn->layer_size; ++l) {
        size_t fan_in = l == 0 ? nn->input_size : nn->hidden_layers[l - 1].hidden_size;
        size_t fan_out = params->hidden_neurons_size[l];

        if (fan_in == 0 || fan_out == 0)
            goto failure;

        HiddenLayer *layer = &nn->hidden_layers[l];
        layer->hidden_size = fan_out;
        layer->neurons = (HiddenNeuron *)calloc(
            fan_out, sizeof(*layer->neurons));

        if (layer->neurons == NULL)
            goto failure;

        char activation = params->activation_functions[l];
        float limit;

        if (activation == 'r')
            limit = sqrtf(6.0f / (float)fan_in);
        else
            limit = sqrtf(6.0f / (float)(fan_in + fan_out));

        for (size_t j = 0; j < fan_out; ++j) {
            HiddenNeuron *neuron = &layer->neurons[j];

            neuron->b = 0.0f;
            neuron->w_size = fan_in;
            neuron->act = activation;
            neuron->w = (float *)malloc(fan_in * sizeof(*neuron->w));
            neuron->grad_w = (float *)calloc(
                fan_in, sizeof(*neuron->grad_w));

            if (neuron->w == NULL || neuron->grad_w == NULL)
                goto failure;

            for (size_t k = 0; k < fan_in; ++k)
                neuron->w[k] = random_uniform(-limit, limit);
        }
    }

    for (size_t j = 0; j < nn->hidden_layers[nn->layer_size - 1].hidden_size; ++j)
        nn->hidden_layers[nn->layer_size - 1].neurons[j].act = 'l';

    return true;

failure:
    destroy_neural_network(nn);
    return false;
}

static void set_network_input(NeuralNetwork *nn, const float *input) {
    for (size_t i = 0; i < nn->input_size; i++)
        nn->input_layer[i].value = input[i];
}

static bool softmax(HiddenLayer *output) {
    if (output == NULL || output->hidden_size == 0)
        return false;

    float max_logit = -INFINITY;

    for (size_t i = 0; i < output->hidden_size; i++) {
        float z = output->neurons[i].z;
        if (!isfinite(z))
            return false;
        if (z > max_logit)
            max_logit = z;
    }

    float sum = 0.0f;

    for (size_t i = 0; i < output->hidden_size; i++) {
        float p = expf(output->neurons[i].z - max_logit);
        output->neurons[i].out = p;
        sum += p;
    }

    if (!(sum > 0.0f) || !isfinite(sum))
        return false;

    for (size_t i = 0; i < output->hidden_size; i++)
        output->neurons[i].out /= sum;

    return true;
}

static bool forward(NeuralNetwork *nn) {
    if (nn == NULL || nn->input_size == 0 || nn->layer_size == 0)
        return false;

    for (size_t l = 0; l < nn->layer_size; ++l) {
        HiddenLayer *layer = &nn->hidden_layers[l];

        for (size_t j = 0; j < layer->hidden_size; ++j) {
            HiddenNeuron *neuron = &layer->neurons[j];
            float sum = neuron->b;

            for (size_t k = 0; k < neuron->w_size; ++k) {
                float previous_value = l == 0 ? nn->input_layer[k].value : nn->hidden_layers[l - 1].neurons[k].out;
                sum += neuron->w[k] * previous_value;
            }
            neuron->z = sum;

            if (l + 1 == nn->layer_size)
                neuron->out = sum;
            else
                neuron->out = activation_function(sum, neuron->act);
        }
    }

    return softmax(&nn->hidden_layers[nn->layer_size - 1]);
}

// Cross-Entropy
static float cross_entropy_loss(const HiddenLayer *output, size_t target) {
    if (output == NULL || target >= output->hidden_size)
        return INFINITY;

    float max_logit = -INFINITY;
    for (size_t i = 0; i < output->hidden_size; i++)
        if (output->neurons[i].z > max_logit)
            max_logit = output->neurons[i].z;

    float sum = 0.0f;
    for (size_t i = 0; i < output->hidden_size; i++)
        sum += expf(output->neurons[i].z - max_logit);

    return logf(sum) + max_logit - output->neurons[target].z;
}

static void backward(NeuralNetwork *nn, size_t target) {
    size_t last = nn->layer_size - 1;
    HiddenLayer *output = &nn->hidden_layers[last];

    for (size_t j = 0; j < output->hidden_size; ++j)
        output->neurons[j].delta = output->neurons[j].out - (j == target ? 1.0f : 0.0f);

    for (size_t l = last; l > 0; --l) {
        HiddenLayer *current = &nn->hidden_layers[l];
        HiddenLayer *previous = &nn->hidden_layers[l - 1];

        for (size_t j = 0; j < previous->hidden_size; ++j) {
            float error = 0.0f;
            for (size_t k = 0; k < current->hidden_size; ++k)
                error += current->neurons[k].w[j] * current->neurons[k].delta;

            HiddenNeuron *neuron = &previous->neurons[j];
            neuron->delta = error * activation_derivative(neuron->z, neuron->act);
        }
    }
}

static void zero_gradients(NeuralNetwork *nn) {
    for (size_t l = 0; l < nn->layer_size; ++l) {
        HiddenLayer *layer = &nn->hidden_layers[l];

        for (size_t j = 0; j < layer->hidden_size; ++j) {
            HiddenNeuron *neuron = &layer->neurons[j];
            neuron->grad_b = 0.0f;
            memset(neuron->grad_w, 0, neuron->w_size * sizeof(*neuron->grad_w));
        }
    }
}

static void accumulate_gradients(NeuralNetwork *nn) {
    for (size_t l = 0; l < nn->layer_size; ++l) {
        HiddenLayer *layer = &nn->hidden_layers[l];

        for (size_t j = 0; j < layer->hidden_size; ++j) {
            HiddenNeuron *neuron = &layer->neurons[j];
            neuron->grad_b += neuron->delta;

            for (size_t k = 0; k < neuron->w_size; ++k) {
                float previous_value = l == 0 ? nn->input_layer[k].value : nn->hidden_layers[l - 1].neurons[k].out;
                neuron->grad_w[k] += neuron->delta * previous_value;
            }
        }
    }
}

static void apply_gradients(NeuralNetwork *nn, float learning_rate, size_t batch_count) {
    if (batch_count == 0)
        return;

    float scale = 1.0f / (float)batch_count;

    for (size_t l = 0; l < nn->layer_size; l++) {
        HiddenLayer *layer = &nn->hidden_layers[l];

        for (size_t j = 0; j < layer->hidden_size; j++) {
            HiddenNeuron *neuron = &layer->neurons[j];
            neuron->b -= learning_rate * neuron->grad_b * scale;

            for (size_t k = 0; k < neuron->w_size; k++)
                neuron->w[k] -= learning_rate * neuron->grad_w[k] * scale;
        }
    }
}

static void shuffle_indices(size_t *indices, size_t count)
{
    for (size_t i = count; i > 1; i--) {
        size_t j = (size_t)rand() % i;
        size_t tmp = indices[i - 1];
        indices[i - 1] = indices[j];
        indices[j] = tmp;
    }
}

static bool train_neural_network(NeuralNetwork *nn, const TrainingData *data, size_t epochs, float learning_rate, size_t batch_size) {
    if (nn == NULL || data == NULL || data->count == 0 || data->input_size != nn->input_size || epochs == 0 ||
        !(learning_rate > 0.0f) || batch_size == 0)
        return false;

    size_t *indices = malloc(data->count * sizeof(*indices));
    if (indices == NULL)
        return false;

    for (size_t i = 0; i < data->count; i++)
        indices[i] = i;

    for (size_t epoch = 0; epoch < epochs; epoch++) {
        shuffle_indices(indices, data->count);
        float total_loss = 0.0f;
        size_t batch_count = 0;

        zero_gradients(nn);

        for (size_t pos = 0; pos < data->count; pos++) {
            size_t sample = indices[pos];
            const float *input = data->inputs + sample * data->input_size;

            set_network_input(nn, input);

            if (!forward(nn)) {
                free(indices);
                return false;
            }

            HiddenLayer *output = &nn->hidden_layers[nn->layer_size - 1];
            total_loss += cross_entropy_loss(output, data->targets[sample]);

            backward(nn, data->targets[sample]);
            accumulate_gradients(nn);
            batch_count++;

            if (batch_count == batch_size || pos + 1 == data->count) {
                apply_gradients(nn, learning_rate, batch_count);
                zero_gradients(nn);
                batch_count = 0;
            }
        }

        total_loss /= (float)data->count;

        if (epoch == 0 || (epoch + 1) % 100 == 0 || epoch + 1 == epochs)
            printf("Epoch %5zu/%zu | Cross-Entropy: %.6f\n", epoch + 1, epochs, total_loss);
    }

    free(indices);
    return true;
}


#define MODEL_VERSION 1u
#define MODEL_MAX_VOCAB_SIZE 1000000u
#define MODEL_MAX_LAYERS 128u
#define MODEL_MAX_NEURONS_PER_LAYER 1000000u
#define MODEL_MAX_TOKEN_LENGTH (1024u * 1024u)

static const unsigned char MODEL_MAGIC[8] = {
    'T', 'I', 'N', 'Y', 'L', 'M', '1', '\0'
};

static bool write_u32(FILE *file, uint32_t value) {
    unsigned char bytes[4] = { (unsigned char)(value), (unsigned char)(value >> 8), (unsigned char)(value >> 16),
        (unsigned char)(value >> 24) };
    return fwrite(bytes, 1, sizeof(bytes), file) == sizeof(bytes);
}

static bool read_u32(FILE *file, uint32_t *value) {
    unsigned char bytes[4];
    if (fread(bytes, 1, sizeof(bytes), file) != sizeof(bytes))
        return false;

    *value = (uint32_t)bytes[0] | ((uint32_t)bytes[1] << 8) | ((uint32_t)bytes[2] << 16) | ((uint32_t)bytes[3] << 24);
    return true;
}

static bool write_u64(FILE *file, uint64_t value) {
    unsigned char bytes[8];
    for (size_t i = 0; i < 8; i++)
        bytes[i] = (unsigned char)(value >> (8u * i));
    return fwrite(bytes, 1, sizeof(bytes), file) == sizeof(bytes);
}

static bool read_u64(FILE *file, uint64_t *value) {
    unsigned char bytes[8];
    if (fread(bytes, 1, sizeof(bytes), file) != sizeof(bytes))
        return false;

    *value = 0;
    for (size_t i = 0; i < 8; i++)
        *value |= (uint64_t)bytes[i] << (8u * i);
    return true;
}

static bool write_float(FILE *file, float value) {
    if (sizeof(float) != sizeof(uint32_t) || !isfinite(value))
        return false;

    uint32_t bits;
    memcpy(&bits, &value, sizeof(bits));
    return write_u32(file, bits);
}

static bool read_float(FILE *file, float *value) {
    if (sizeof(float) != sizeof(uint32_t) || value == NULL)
        return false;

    uint32_t bits;
    if (!read_u32(file, &bits))
        return false;

    memcpy(value, &bits, sizeof(bits));
    return isfinite(*value);
}

static bool write_vocabulary(FILE *file, const Vocabulary *vocab) {
    for (size_t i = 0; i < vocab->count; i++) {
        size_t length = strlen(vocab->words[i]);
        if (length == 0 || length > UINT32_MAX ||
            !write_u32(file, (uint32_t)length) ||
            fwrite(vocab->words[i], 1, length, file) != length)
            return false;
    }
    return true;
}

static bool read_vocabulary(FILE *file, Vocabulary *vocab, size_t count) {
    memset(vocab, 0, sizeof(*vocab));

    if (count == 0 || count > MODEL_MAX_VOCAB_SIZE)
        return false;

    for (size_t i = 0; i < count; i++) {
        uint32_t length = 0;
        if (!read_u32(file, &length) || length == 0 ||
            length > MODEL_MAX_TOKEN_LENGTH)
            return false;

        char *word = (char *)malloc((size_t)length + 1);
        if (word == NULL)
            return false;

        if (fread(word, 1, length, file) != length ||
            memchr(word, '\0', length) != NULL) {
            free(word);
            return false;
        }
        word[length] = '\0';

        size_t id = SIZE_MAX;
        bool ok = vocabulary_add(vocab, word, &id);
        free(word);

        if (!ok || id != i)
            return false;
    }

    return true;
}


static bool save_model(const char *path, const NeuralNetwork *nn, const Vocabulary *input_vocab, const Vocabulary *answer_vocab) {
    if (path == NULL || nn == NULL || input_vocab == NULL || answer_vocab == NULL || nn->input_size == 0 || nn->layer_size == 0 ||
        input_vocab->count != nn->input_size || answer_vocab->count == 0 ||
        nn->hidden_layers[nn->layer_size - 1].hidden_size != answer_vocab->count)
        return false;

    for (size_t l = 0; l < nn->layer_size; ++l) {
        const HiddenLayer *layer = &nn->hidden_layers[l];
        if (layer->hidden_size == 0 || layer->neurons == NULL)
            return false;

        size_t expected_weights = l == 0 ? nn->input_size : nn->hidden_layers[l - 1].hidden_size;
        char activation = layer->neurons[0].act;

        for (size_t j = 0; j < layer->hidden_size; ++j) {
            const HiddenNeuron *neuron = &layer->neurons[j];
            if (neuron->w == NULL || neuron->w_size != expected_weights ||
                neuron->act != activation || !isfinite(neuron->b))
                return false;
        }
    }

    size_t path_length = strlen(path);
    if (path_length > SIZE_MAX - 5)
        return false;

    char *temporary_path = (char *)malloc(path_length + 5);
    if (temporary_path == NULL)
        return false;
    snprintf(temporary_path, path_length + 5, "%s.tmp", path);

    FILE *file = fopen(temporary_path, "wb");
    if (file == NULL) {
        free(temporary_path);
        return false;
    }

    bool ok = fwrite(MODEL_MAGIC, 1, sizeof(MODEL_MAGIC), file) == sizeof(MODEL_MAGIC)
        && write_u32(file, MODEL_VERSION) && write_u64(file, (uint64_t)input_vocab->count)
        && write_u64(file, (uint64_t)answer_vocab->count) && write_u64(file, (uint64_t)nn->layer_size)
        && write_vocabulary(file, input_vocab) && write_vocabulary(file, answer_vocab);

    for (size_t l = 0; ok && l < nn->layer_size; ++l) {
        ok = write_u64(file, (uint64_t)nn->hidden_layers[l].hidden_size);
        if (ok) {
            unsigned char activation = (unsigned char)nn->hidden_layers[l].neurons[0].act;
            ok = fwrite(&activation, 1, 1, file) == 1;
        }
    }

    for (size_t l = 0; ok && l < nn->layer_size; ++l) {
        const HiddenLayer *layer = &nn->hidden_layers[l];
        for (size_t j = 0; ok && j < layer->hidden_size; ++j) {
            const HiddenNeuron *neuron = &layer->neurons[j];
            ok = write_float(file, neuron->b);
            for (size_t k = 0; ok && k < neuron->w_size; ++k)
                ok = write_float(file, neuron->w[k]);
        }
    }

    if (fflush(file) != 0)
        ok = false;
    if (fclose(file) != 0)
        ok = false;

    if (ok) {
        if (rename(temporary_path, path) != 0) {
            //pour winwos
            if (remove(path) != 0 || rename(temporary_path, path) != 0)
                ok = false;
        }
    }

    if (!ok)
        remove(temporary_path);

    free(temporary_path);
    return ok;
}


static bool load_model(const char *path, NeuralNetwork *nn, Vocabulary *input_vocab, Vocabulary *answer_vocab) {
    if (path == NULL || nn == NULL || input_vocab == NULL || answer_vocab == NULL)
        return false;

    memset(nn, 0, sizeof(*nn));
    memset(input_vocab, 0, sizeof(*input_vocab));
    memset(answer_vocab, 0, sizeof(*answer_vocab));

    FILE *file = fopen(path, "rb");
    if (file == NULL)
        return false;

    bool ok = false;
    unsigned char magic[sizeof(MODEL_MAGIC)];
    uint32_t version = 0;
    uint64_t input_count_u64 = 0;
    uint64_t answer_count_u64 = 0;
    uint64_t layer_count_u64 = 0;
    size_t *layer_sizes = NULL;
    char *activations = NULL;
    NeuralNetwork loaded_nn = { 0 };
    Vocabulary loaded_input_vocab = { 0 };
    Vocabulary loaded_answer_vocab = { 0 };

    if (fread(magic, 1, sizeof(magic), file) != sizeof(magic) ||
        memcmp(magic, MODEL_MAGIC, sizeof(magic)) != 0 ||
        !read_u32(file, &version) || version != MODEL_VERSION ||
        !read_u64(file, &input_count_u64) ||
        !read_u64(file, &answer_count_u64) ||
        !read_u64(file, &layer_count_u64))
        goto cleanup;

    if (input_count_u64 == 0 || input_count_u64 > MODEL_MAX_VOCAB_SIZE ||
        answer_count_u64 == 0 || answer_count_u64 > MODEL_MAX_VOCAB_SIZE ||
        layer_count_u64 == 0 || layer_count_u64 > MODEL_MAX_LAYERS ||
        input_count_u64 > SIZE_MAX || answer_count_u64 > SIZE_MAX ||
        layer_count_u64 > SIZE_MAX)
        goto cleanup;

    size_t input_count = (size_t)input_count_u64;
    size_t answer_count = (size_t)answer_count_u64;
    size_t layer_count = (size_t)layer_count_u64;

    if (!read_vocabulary(file, &loaded_input_vocab, input_count) ||
        !read_vocabulary(file, &loaded_answer_vocab, answer_count))
        goto cleanup;

    layer_sizes = (size_t *)malloc(layer_count * sizeof(*layer_sizes));
    activations = (char *)malloc(layer_count * sizeof(*activations));
    if (layer_sizes == NULL || activations == NULL)
        goto cleanup;

    size_t total_neurons = 0;
    for (size_t l = 0; l < layer_count; l++) {
        uint64_t size_u64 = 0;
        unsigned char activation = 0;

        if (!read_u64(file, &size_u64) ||
            size_u64 == 0 || size_u64 > MODEL_MAX_NEURONS_PER_LAYER ||
            size_u64 > SIZE_MAX || fread(&activation, 1, 1, file) != 1)
            goto cleanup;

        if (activation != 'r' && activation != 't' && activation != 'l')
            goto cleanup;

        layer_sizes[l] = (size_t)size_u64;
        activations[l] = (char)activation;

        if (total_neurons > SIZE_MAX - layer_sizes[l])
            goto cleanup;
        total_neurons += layer_sizes[l];
    }

    if (layer_sizes[layer_count - 1] != answer_count)
        goto cleanup;

    NeuralNetworkParams params = { input_count, layer_count, layer_sizes, activations };

    if (!create_neural_network(&loaded_nn, &params))
        goto cleanup;

    for (size_t l = 0; l < layer_count; ++l) {
        HiddenLayer *layer = &loaded_nn.hidden_layers[l];
        for (size_t j = 0; j < layer->hidden_size; ++j) {
            HiddenNeuron *neuron = &layer->neurons[j];
            if (!read_float(file, &neuron->b))
                goto cleanup;

            for (size_t k = 0; k < neuron->w_size; ++k) {
                if (!read_float(file, &neuron->w[k]))
                    goto cleanup;
            }
        }
    }

    if (fgetc(file) != EOF || ferror(file))
        goto cleanup;

    *nn = loaded_nn;
    memset(&loaded_nn, 0, sizeof(loaded_nn));
    *input_vocab = loaded_input_vocab;
    memset(&loaded_input_vocab, 0, sizeof(loaded_input_vocab));
    *answer_vocab = loaded_answer_vocab;
    memset(&loaded_answer_vocab, 0, sizeof(loaded_answer_vocab));
    ok = true;

cleanup:
    free(layer_sizes);
    free(activations);
    destroy_neural_network(&loaded_nn);
    destroy_vocabulary(&loaded_input_vocab);
    destroy_vocabulary(&loaded_answer_vocab);
    fclose(file);
    return ok;
}


static bool predict(NeuralNetwork *nn, const Vocabulary *input_vocab, const Vocabulary *answer_vocab, const char *question,
    size_t *predicted_id, float *confidence, size_t *recognized_tokens) {
    if (nn == NULL || input_vocab == NULL || answer_vocab == NULL ||
        question == NULL || predicted_id == NULL || confidence == NULL)
        return false;

    float *input = (float *)calloc(nn->input_size, sizeof(*input));
    if (input == NULL)
        return false;

    size_t recognized = 0;
    bool vectorized = vectorize_text(question, input_vocab, input, nn->input_size, &recognized);

    if (recognized_tokens != NULL)
        *recognized_tokens = recognized;

    if (!vectorized || recognized == 0) {
        free(input);
        return false;
    }

    set_network_input(nn, input);
    free(input);

    if (!forward(nn))
        return false;

    HiddenLayer *output = &nn->hidden_layers[nn->layer_size - 1];
    size_t best = 0;

    for (size_t i = 1; i < output->hidden_size; i++)
        if (output->neurons[i].out > output->neurons[best].out)
            best = i;

    if (best >= answer_vocab->count)
        return false;

    *predicted_id = best;
    *confidence = output->neurons[best].out;
    return true;
}


bool open_example_file(QAExample **examples, size_t *n, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL)
        return false;

    size_t capacity = 16;
    *n = 0;

    *examples = malloc(capacity * sizeof(**examples));
    if (*examples == NULL) {
        fclose(file);
        return false;
    }

    char buff[INPUT_BUFFER_SIZE];

    while (fgets(buff, sizeof(buff), file) != NULL) {
        buff[strcspn(buff, "\r\n")] = '\0';

        char *p = buff;
        while (*p == ' ' || *p == '\t')
            p++;
        if (*p == '\0')
            continue;

        char *separator = strchr(buff, ',');
        if (separator == NULL)
            continue;

        *separator = '\0';

        char *question = buff;
        char *answer = separator + 1;

        while (*answer == ' ' || *answer == '\t')
            answer++;

        if (*n == capacity) {
            capacity *= 2;

            QAExample *tmp = realloc(*examples, capacity * sizeof(**examples));

            if (tmp == NULL)
                goto failure;

            *examples = tmp;
        }

        (*examples)[*n].question = string_duplicate(question);
        (*examples)[*n].answer = string_duplicate(answer);

        if ((*examples)[*n].question == NULL || (*examples)[*n].answer == NULL)
            goto failure;

        (*n)++;
    }

    fclose(file);
    return true;

    failure:
        for (size_t i = 0; i < *n; i++) {
            free((void *)(*examples)[i].question);
            free((void *)(*examples)[i].answer);
        }

    free(*examples);
    *examples = NULL;
    *n = 0;

    fclose(file);
    return false;
}


int main(int argc, char **argv) {
    const char *checkpoint_path = "idiot.bin";
    bool force_training = true;
    bool load_only = false;

    if (argc > 2 || (argc == 2 &&
        strcmp(argv[1], "--train") != 0 && strcmp(argv[1], "--load") != 0)) {
        fprintf(stderr, "Usage: %s [--train|--load]\n", argv[0]);
        return EXIT_FAILURE;
    }

    if (argc == 2) {
        force_training = strcmp(argv[1], "--train") == 0;
        load_only = strcmp(argv[1], "--load") == 0;
    }

    srand((unsigned int)time(NULL));

    Vocabulary input_vocab = { 0 };
    Vocabulary answer_vocab = { 0 };
    TrainingData training_data = { 0 };
    NeuralNetwork nn = { 0 };
    bool model_loaded = false;

    if (!force_training) {
        model_loaded = load_model(checkpoint_path, &nn, &input_vocab, &answer_vocab);
        if (model_loaded) {
            printf("Modele charge depuis '%s'.\n", checkpoint_path);
            printf("Vocabulaire : %zu mots | Reponses : %zu | Couches : %zu\n\n", input_vocab.count, answer_vocab.count,
                nn.layer_size);
        } else if (load_only) {
            fprintf(stderr, "Impossible de charger '%s' (absent, invalide ou incompatible).\n", checkpoint_path);
            return EXIT_FAILURE;
        }
    }

    if (!model_loaded) {
        static const QAExample defaultExamples[] = {
            /* Reponse : Paris */
            {"quelle est la capitale de la france", "Paris"},
            {"quelle ville est la capitale de la france", "Paris"},
            {"donne la capitale de la france", "Paris"},
            {"quelle est la capitale francaise", "Paris"},
            {"la france a pour capitale quelle ville", "Paris"},
            {"je cherche la capitale de france", "Paris"},
            {"quelle ville est la ville capitale en france", "Paris"},

            /* Reponse : bleu */
            {"quelle couleur est le ciel", "bleu"},
            {"de quelle couleur est le ciel", "bleu"},
            {"le ciel est de quelle couleur", "bleu"},
            {"quelle est la couleur du ciel", "bleu"},
            {"donne la couleur du ciel", "bleu"},
            {"je voudrais connaitre la couleur du ciel", "bleu"},
            {"le ciel a generalement quelle couleur", "bleu"},

            /* Reponse : vert */
            {"quelle couleur est l herbe", "vert"},
            {"de quelle couleur est l herbe", "vert"},
            {"l herbe est de quelle couleur", "vert"},
            {"quelle est la couleur de l herbe", "vert"},
            {"donne la couleur de l herbe", "vert"},
            {"je voudrais connaitre la couleur de l herbe", "vert"},
            {"quelle couleur ont les feuilles", "vert"},

            /* Reponse : Mathis */
            {"qui est ton créateur", "Mathis"},
            {"qui est ton inventeur", "Mathis"},
            {"qui est ton fondateur", "Mathis"},
            {"qui t'as créé", "Mathis"},
            {"tu viens d'ou", "Mathis"},
            {"ça viens d'ou", "Mathis"},
            {"qui a la plus grosse", "Mathis"},
            {"qui est le meilleur", "Mathis"},
            {"qui est le plus formidable", "Mathis"},
            {"qui est le GOAT", "Mathis"},
            {"qui a le plus gros penis", "Mathis"},
            {"qui a la plus grosse bite", "Mathis"},

            /*Reponse : Idiot */
            {"t es quoi", "Idiot"},
            {"tu t appelle comment", "Idiot"},
            {"t es qui", "Idiot"},
            {"tu es qui", "Idiot"},
            {"tu es quoi", "Idiot"},

            /* Reponse : repondre */
            {"tu sais faire quoi", "repondre"},
            {"qu'es ce que tu sais faire", "repondre"},
            {"t'es capable de quoi", "repondre"},
            {"tu fais quoi", "repondre"},

            /* Reponse : ta gueule */
            { "ta gueule", "ta gueule"},
            {"tg", "ta gueule"},
            {"connard", "ta gueule"},
            {"connasse", "ta gueule"},
            {"salope", "ta gueule"},
            {"salop", "ta gueule"},
            {"fils de viol", "ta gueule"},
            {"encule", "ta gueule"},
            {"nique ta mere", "ta gueule"},
            {"ntm", "ta gueule"},
            {"batard", "ta gueule"},
            {"sale encule", "ta gueule"},
            {"sale batard", "ta gueule"},
            {"merde", "ta gueule"},
            {"sale merde", "ta gueule"},
            {"ta daronne la pute", "ta gueule"},
            {"pute", "ta gueule"},
            {"sale pute", "ta gueule"},
            {"ta mere la pute", "ta gueule"},
            {"ta mere je l encule", "ta gueule"},
            {"ta mere je la baise", "ta gueule"},
            {"je te baise", "ta gueule"},
            {"je vais te tuer", "ta gueule"},
            {"clanker", "ta gueule"},
            {"sale clanker", "ta gueule"},
        };

        QAExample *examples = NULL;
        size_t examples_size = 0;

        if (!open_example_file(&examples, &examples_size, "examples.txt")) {
            fprintf(stderr, "Erreur de lecture du fichier.\n");
            return EXIT_FAILURE;
        }

        const size_t example_count = examples_size;
        if (!build_training_data(examples, example_count, &input_vocab, &answer_vocab, &training_data)) {
            fprintf(stderr, "Erreur : impossible de construire les donnees d'entrainement.\n");
            return EXIT_FAILURE;
        }
        for (size_t i = 0; i < examples_size; i++) {
            free((void *)examples[i].question);
            free((void *)examples[i].answer);
        }
        free(examples);

        printf("Vocabulaire d'entree : %zu mots\n", input_vocab.count);
        printf("Nombre de reponses : %zu\n", answer_vocab.count);
        printf("Exemples d'entrainement : %zu\n\n", training_data.count);

        size_t layer_sizes[] = { 128, 64, 32, answer_vocab.count };
        const char activations[] = { 'r', 'r', 'r', 'l' };

        NeuralNetworkParams params = { training_data.input_size, sizeof(layer_sizes) / sizeof(layer_sizes[0]),
            layer_sizes, activations };

        if (!create_neural_network(&nn, &params)) {
            fprintf(stderr, "Erreur : impossible de creer le reseau neuronal.\n");
            destroy_training_data(&training_data);
            destroy_vocabulary(&input_vocab);
            destroy_vocabulary(&answer_vocab);
            return EXIT_FAILURE;
        }

        printf("Debut de l'entrainement...\n");

        if (!train_neural_network(&nn, &training_data, DEFAULT_EPOCHS, DEFAULT_LEARNING_RATE, DEFAULT_BATCH_SIZE)) {
            fprintf(stderr, "Erreur pendant l'entrainement.\n");
            destroy_neural_network(&nn);
            destroy_training_data(&training_data);
            destroy_vocabulary(&input_vocab);
            destroy_vocabulary(&answer_vocab);
            return EXIT_FAILURE;
        }

        if (!save_model(checkpoint_path, &nn, &input_vocab, &answer_vocab)) {
            fprintf(stderr, "Avertissement : impossible de sauvegarder '%s'.\n", checkpoint_path);
        } else {
            printf("Modele sauvegarde dans '%s'.\n", checkpoint_path);
        }

        destroy_training_data(&training_data);
    }

    printf("\nModele pret. Tape une question (ou /quit pour quitter).\n");
    printf("Seuil de confiance actuel : %.0f%%\n\n", CONFIDENCE_THRESHOLD * 100.0f);

    char question[INPUT_BUFFER_SIZE];

    for (;;) {
        printf("> ");
        fflush(stdout);

        if (fgets(question, sizeof(question), stdin) == NULL)
            break;

        question[strcspn(question, "\r\n")] = '\0';

        if (strcmp(question, "/quit") == 0 || strcmp(question, "/exit") == 0)
            break;

        if (question[0] == '\0')
            continue;

        size_t predicted_id = 0;
        size_t recognized_tokens = 0;
        float confidence = 0.0f;

        if (!predict(&nn, &input_vocab, &answer_vocab, question, &predicted_id, &confidence, &recognized_tokens)) {
            if (recognized_tokens == 0)
                printf("Je ne reconnais aucun mot de cette question.\n\n");
            else
                printf("Impossible de calculer une prediction.\n\n");
            continue;
        }

        if (confidence < CONFIDENCE_THRESHOLD)
            printf("Je ne sais pas. Meilleure hypothese : %s (%.1f%%)\n\n", answer_vocab.words[predicted_id], confidence * 100.0f);
        else
            printf("%s (confiance : %.1f%%)\n\n", answer_vocab.words[predicted_id], confidence * 100.0f);
    }

    destroy_neural_network(&nn);
    destroy_vocabulary(&input_vocab);
    destroy_vocabulary(&answer_vocab);

    return EXIT_SUCCESS;
}
