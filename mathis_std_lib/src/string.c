#include "../include/string.h"

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


void string_init(String *s) {
    s->data = NULL;
    s->length = 0;
    s->capacity = 0;
}

bool string_reserve(String *s, const size_t needed) {
    if (needed <= s->capacity)
        return true;

    size_t cap = s->capacity ? s->capacity : 16;

    while (cap < needed) {
        if (cap > SIZE_MAX / 2) {
            cap = needed;
            break;
        }
        cap *= 2;
    }

    char *p = realloc(s->data, cap);
    if (!p)
        return false;

    s->data = p;
    s->capacity = cap;
    return true;
}

bool string_append_n(String *s, const char *text, const size_t n) {
    if (n > SIZE_MAX - s->length - 1)
        return false;

    const size_t needed = s->length + n + 1;

    if (text >= s->data && text < s->data + s->length + 1) {
        char *tmp = malloc(n);
        if (n && !tmp)
            return false;
        if (n)
            memcpy(tmp, text, n);

        if (!string_reserve(s, needed)) {
            free(tmp);
            return false;
        }

        if (n)
            memcpy(s->data + s->length, tmp, n);
        free(tmp);
    } else {
        if (!string_reserve(s, needed))
            return false;
        if (n)
            memcpy(s->data + s->length, text, n);
    }

    s->length += n;
    s->data[s->length] = '\0';
    return true;
}

bool string_append(String *s, const char *text) {
    return string_append_n(s, text, strlen(text));
}

bool string_pop(String *s, char *out, const size_t index) {
    if (!s->length)
        return false;
    if (out == NULL)
        return false;
    if (index >= s->length)
        return false;

    *out = s->data[index];
    s->length--;

    return true;
}

bool string_pop_back(String *s) {
    if (!s->length)
        return false;

    s->length--;
    return true;
}

void string_free(String *s) {
    free(s->data);
    string_init(s);
}
