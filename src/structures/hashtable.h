#pragma once
#include <stdint.h>
#include <stdbool.h>

#define HASHTABLE_DEFAULT_SIZE 16
#define HASHTABLE_LOAD_CAP 0.7f
#define HASHTABLE_STRING -1

/// Key Value pair
typedef struct pair_t {
    void *key;
    void *value;
    uint32_t size;
    struct pair_t *previous, *next;
} pair_t;

/// Create and initialize a key/value pair
/// Copies the value to a new location
pair_t *pair_create(void *key, uint32_t key_size, void *value, uint32_t value_size);
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
    int key_size;
    uint64_t bucket_count;
    uint64_t pair_count;
    bucket_t *buckets;
} hashtable_t;

/// Create and allocate a hashtable indexed by strings
/// Default size is 16
hashtable_t hashtable_string(void);
/// Create and allocate a hashtable indexed by an arbitrary type
/// Default size is 16
hashtable_t hashtable_arbitrary(uint32_t key_size);
/// Clean up after a hashtable
void hashtable_delete(hashtable_t *this);
/// Reset a hash table
void hashtable_reset(hashtable_t *this);

/// Insert a key/pair value into a hashtable
/// Returns true on success, false on failure
void *hashtable_insert(hashtable_t *this, void *key, void *value, uint32_t size);
/// Get a value from a hashtable
/// Returns null if not found
void *hashtable_get(hashtable_t *this, void *key);
/// Get all values of a hashtable.
/// Returns an array of pointers, which must be freed.
pair_t **hashtable_pairs(hashtable_t *this, uint32_t *count);
/// Remove a key from a hashtable
void hashtable_remove(hashtable_t *this, void *key);
/// Calculate the load of a hashtable
double hashtable_calculate_load(hashtable_t *this, uint64_t count);
/// Rehash a hashtable, reorganizing its elements
/// Probably should only be used internally
void hashtable_rehash(hashtable_t *this, uint64_t count);
/// Hash a key using jhash or jhash_str depending on whether the table is arbitrary or not.
uint32_t hashtable_hash(hashtable_t *this, void *key);