#include "../mathis_std_lib.h"

#include <stdio.h>

int main() {
    printf("[INFO] - test_0.c \n");

    printf("----------------\n");
    printf("[INFO] - Vector \n");

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

    printf("----------------\n");
    printf("[INFO] - String \n");

    String name;
    string_init(&name);

    string_append(&name, "Hello");
    string_append(&name, " ");
    string_append(&name, "World");

    string_pop_back(&name);

    string_free(&name);

    return 0;
}