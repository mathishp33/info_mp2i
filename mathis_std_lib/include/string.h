#ifndef INFO_MP2I_STRING_H
#define INFO_MP2I_STRING_H

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>


typedef struct {
    char *data;
    size_t length;
    size_t capacity;
} String;

void string_init(String *s);

bool string_reserve(String *s, size_t needed);

bool string_append_n(String *s, const char *text, size_t n);

bool string_append(String *s, const char *text);

bool string_pop(String *s, char *out, size_t index);

bool string_pop_back(String *s);

void string_free(String *s);




#endif