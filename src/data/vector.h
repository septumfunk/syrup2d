//? septumfunk 2024
#pragma once

/// Simple generic-esque vector implementation.
typedef struct vector_t {
    void *data;
    unsigned int length;
    unsigned long long element_size;
} vector_t;

/// Creates a vector_t, or dynamic array.
/// This function allocates on the heap. Please ensure to call its matching cleanup when it's no longer needed!
vector_t vector_create(unsigned long long element_size);

/// Destroys a vector_t and all of its elements.
void vector_delete(vector_t *vector);

/// Gets vector value at specified index.
/// Please cast this to the desired type.
void *vector_get(vector_t *vector, unsigned int index);

/// Pushes an element to the end of the vector.
void *vector_push(vector_t *vector, void *data);

/// Removes a vector element at the specified index.
void vector_remove(vector_t *vector, unsigned int index);
