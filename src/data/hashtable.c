//? septumfunk
#include "hashtable.h"
#include <stdlib.h>
#include <string.h>

pair_t *pair_create(const char *key, void *value, uint32_t size) {
    pair_t *p = calloc(1, sizeof(pair_t));

    p->key = calloc(1, strlen(key) + 1);
    strcpy(p->key, key);

    p->value = calloc(1, size);
    memcpy(p->value, value, size);

    return p;
}

void pair_delete(pair_t *this) {
    if (this->next) this->next->previous = this->previous;
    if (this->previous) this->previous->next = this->next;
    free(this->key);
    free(this->value);
    free(this);
}

pair_t *pair_push(pair_t *list, pair_t *pair) {
    if (list == NULL)
        return pair;
    for (pair_t *node = list; node->previous != NULL; node = node->previous)
        node->previous = pair;
    return pair;
}

hashtable_t hashtable_create() {
    return (hashtable_t) {
        HASHTABLE_DEFAULT_SIZE,
        0,
        calloc(HASHTABLE_DEFAULT_SIZE, sizeof(bucket_t)),
    };
}

void hashtable_delete(hashtable_t *this) {
    for (bucket_t *buck = this->buckets; buck < this->buckets + this->bucket_count; ++buck) {
        while (buck->pair != NULL) {
            pair_t *next = buck->pair->next;
            pair_delete(buck->pair);
            buck->pair = next;
        }
    }
}

void hashtable_insert(hashtable_t *this, const char *key, void *value, uint32_t size) {
    uint32_t hash = _jhash(key) % this->bucket_count;
    this->buckets[hash].pair = pair_push(this->buckets[hash].pair, pair_create(key, value, size));
    this->pair_count++;
    if (hashtable_calculate_load(this) > HASHTABLE_LOAD_CAP)
        hashtable_rehash(this, this->bucket_count * 2);
}

void *hashtable_get(hashtable_t *this, const char *key) {
    uint32_t hash = _jhash(key) % this->bucket_count;
    for (pair_t *pair = this->buckets[hash].pair; pair != NULL; pair = pair->next)
        if (pair->key == key)
            return pair->value;
    return NULL;
}

void hashtable_remove(hashtable_t *this, const char *key) {
    pair_t *pair = hashtable_get(this, key);
    if (pair != NULL) {
        if (hashtable_calculate_load(this) <= HASHTABLE_LOAD_CAP)
            hashtable_rehash(this, this->bucket_count / 2);
        pair_delete(pair);
        this->pair_count--;
    }
}

double hashtable_calculate_load(hashtable_t *this) {
    return this->pair_count / this->bucket_count;
}

void hashtable_rehash(hashtable_t *this, uint64_t count) {
    bucket_t *old_buckets = this->buckets;
    this->buckets = calloc(count, sizeof(bucket_t));

    // Rehash process
    for (bucket_t *bucket = old_buckets; bucket < old_buckets + this->bucket_count; ++bucket) {
        for (pair_t *pair = bucket->pair; pair != NULL; pair = pair->next) {
            uint32_t hash = _jhash(pair->key) % count;
            this->buckets[hash].pair = pair_push(this->buckets[hash].pair, pair);
        }
    }

    // Update count and clean
    this->bucket_count = count;
    free(old_buckets);
}

uint32_t _jhash(const char *key) {
    uint32_t hash = 0;
    for (uint64_t i = 0; key[i] != '\0'; ++i) {
        hash += key[i];
        hash += hash << 10;
        hash ^= hash >> 6;
    }
    hash += hash << 3;
    hash ^= hash >> 11;
    hash += hash << 15;
    return hash;
}
