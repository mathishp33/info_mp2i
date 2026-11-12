#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>

#include "vector.h"

int main() {
    printf("----------  INITIALISATION  ---------- \n");


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