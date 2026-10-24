#include "../include/vector.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>


bool vector_init(Vector *v, const size_t elem_size) {
    if (!elem_size)
        return false;
    v->data = NULL;
    v->size = 0;
    v->capacity = 0;
    v->elem_size = elem_size;
    return true;
}

bool vector_reserve(Vector *v, const size_t new_capacity) {
    if (new_capacity <= v->capacity)
        return true;

    if (new_capacity > (size_t) - 1 / sizeof(int))
        return false;

    void *ptr = realloc(v->data, new_capacity * v->elem_size);

    if (ptr == NULL)
        return false;

    v->data = ptr;
    v->capacity = new_capacity;
    return true;
}

bool vector_push_back(Vector *v, const void *elem) {
    if (!v->elem_size || v->size == SIZE_MAX / v->elem_size)
        return false;

    if (v->size == v->capacity) {
        const size_t new_capacity = v->capacity ? v->capacity * 2 : 4;

        if (new_capacity < v->capacity || !vector_reserve(v, new_capacity)) return false;
    }

    memcpy(v->data + v->size * v->elem_size, elem, v->elem_size);
    v->size++;

    return true;
}

bool vector_pop(Vector *v, void *out, const size_t index) {
    if (!v->size)
        return false;
    if (out == NULL)
        return false;
    if (index >= v->size)
        return false;

    memcpy(out, v->data + index * v->elem_size, v->elem_size);
    v->size--;

    return true;
}

bool vector_pop_back(Vector *v) {
    if (!v->size)
        return false;

    v->size--;
    return true;
}

bool vector_get(const Vector *v, const size_t index, void *out) {
    if (index >= v->size)
        return false;

    memcpy(out, v->data + index * v->elem_size, v->elem_size);

    return true;
}

bool vector_front(const Vector *v, void *out) {
    return vector_get(v, 0, out);
}

bool vector_back(const Vector *v, void *out) {
    return vector_get(v, v->size - 1, out);
}

void vector_free(Vector *v) {
    free(v->data);
    v->data = NULL;
    v->size = v->capacity = 0;
    v->elem_size = 0;
}
