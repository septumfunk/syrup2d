// septumfunk 2024
#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

vector_t vector_create(unsigned long long element_size) {
    return (vector_t){
        .data = NULL,
        .element_size = element_size,
        .length = 0,
    };
}

void vector_delete(vector_t *vector) { free(vector->data); }

void *vector_get(vector_t *vector, unsigned int index) {
    if (index > vector->length - 1) {
        fprintf(stderr, "Index %d is out of range.", index);
        exit(-1);
    }
    return (char *)vector->data + index * (unsigned int)vector->element_size;
}

void *vector_push(vector_t *vector, void *data) {
    vector->length++;
    vector->data = realloc(vector->data, vector->element_size * vector->length);
    memcpy((char *)vector->data + vector->element_size * (vector->length - 1), data, vector->element_size);
    return (char *)vector->data + vector->element_size * (vector->length - 1);
}

void vector_remove(vector_t *vector, unsigned int index) {
    if (index > vector->length - 1) {
        fprintf(stderr, "Index %d is out of range.", index);
        exit(-1);
    }
    void *loc = vector_get(vector, index);
    memcpy(loc, (char *)loc + vector->element_size, (vector->length - 1 - index) * vector->element_size);
    vector->length--;
    vector->data = realloc(vector->data, vector->length * vector->element_size);
}
