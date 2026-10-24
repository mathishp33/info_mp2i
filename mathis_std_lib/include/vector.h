#ifndef INFO_MP2I_VECTOR_H
#define INFO_MP2I_VECTOR_H

#include <stdlib.h>
#include <stdbool.h>


typedef struct {
	unsigned char *data;
    size_t size;
    size_t capacity;
	size_t elem_size;
} Vector;

bool vector_init(Vector *v, size_t elem_size);

bool vector_reserve(Vector *v, size_t new_capacity);

bool vector_push_back(Vector *v, const void *elem);

bool vector_pop(Vector *v, void *out, size_t index);

bool vector_pop_back(Vector *v);

bool vector_get(const Vector *v, size_t index, void *out);

bool vector_front(const Vector *v, void *out);

bool vector_back(const Vector *v, void *out);

void vector_free(Vector *v);

#endif