//? septumfunk
#pragma once
#include <stdint.h>

#define HASHTABLE_DEFAULT_SIZE 16

/// Key Value pair
typedef struct pair_t {
    char *key;
    void *value;
    uint32_t size;
} pair_t;

/// Create and initialize a key/value pair
/// Copies the value to a new location
pair_t *pair_create(const char *key, void *value, uint32_t size);
/// Delete and clean up after a key/value
void pair_delete(pair_t *this);

/// A node in a linked list of pairs
typedef struct hash_node_t {
    pair_t *pair;
    struct hash_node_t *next;
} hash_node_t;

/// A bucket for use in a hashtable
typedef struct bucket_t {
    pair_t *pair;
    hash_node_t *list;
} bucket_t;

/// Hashtable implementation
/// Default size is set to 16
typedef struct hashtable_t {
    uint64_t length;
    bucket_t *buckets;
} hashtable_t;

/// Create and allocate a hashtable
/// Default size is 16
hashtable_t hashtable_create();
/// Clean up after a hashtable
void hashtable_delete(hashtable_t *this);

/// Insert a key/pair value into a hashtable
void hashtable_insert(hashtable_t *this, const char *key, const char *value, uint32_t size);
/// Remove a key from a hashtable
void hashtable_remove(hashtable_t *this, const char *key);
/// Rehash a hashtable, reorganizing its elements
/// Probably should only be used internally
void hashtable_rehash(hashtable_t *this);

/// Jenkin's Hash Function
uint32_t _jhash(const char *key);