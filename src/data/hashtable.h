//? septumfunk
#pragma once
#include <stdint.h>

#define HASHTABLE_DEFAULT_SIZE 16
#define HASHTABLE_LOAD_CAP 0.7f

/// Key Value pair
typedef struct pair_t {
    char *key;
    void *value;
    uint32_t size;
    struct pair_t *previous, *next;
} pair_t;

/// Create and initialize a key/value pair
/// Copies the value to a new location
pair_t *pair_create(const char *key, void *value, uint32_t size);
/// Delete and clean up after a key/value
void pair_delete(pair_t *this);
/// Push a pair to the start of a list
pair_t *pair_push(pair_t *list, pair_t *pair);

/// A bucket for use in a hashtable
typedef struct bucket_t {
    pair_t *pair;
} bucket_t;

/// Hashtable implementation
/// Default size is set to 16
typedef struct hashtable_t {
    uint64_t bucket_count, pair_count;
    bucket_t *buckets;
} hashtable_t;

/// Create and allocate a hashtable
/// Default size is 16
hashtable_t hashtable_create();
/// Clean up after a hashtable
void hashtable_delete(hashtable_t *this);

/// Insert a key/pair value into a hashtable
void hashtable_insert(hashtable_t *this, const char *key, void *value, uint32_t size);
/// Get a value from a hashtable
/// Returns null if not found
void *hashtable_get(hashtable_t *this, const char *key);
/// Remove a key from a hashtable
void hashtable_remove(hashtable_t *this, const char *key);
/// Calculate the load of a hashtable
double hashtable_calculate_load(hashtable_t *this);
/// Rehash a hashtable, reorganizing its elements
/// Probably should only be used internally
void hashtable_rehash(hashtable_t *this, uint64_t count);

/// Jenkin's Hash Function
uint32_t _jhash(const char *key);