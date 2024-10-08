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
    free(this->key);
    free(this->value);
    free(this);
}

hashtable_t hashtable_create() {
    return (hashtable_t) {
        HASHTABLE_DEFAULT_SIZE,
        calloc(HASHTABLE_DEFAULT_SIZE, sizeof(bucket_t)),
    };
}

void hashtable_delete(hashtable_t *this) {
    for (bucket_t *buck = this->buckets; buck < this->buckets + this->length; ++buck) {
        if (buck->pair != NULL) {
            pair_delete(buck->pair);
        }
        if (buck->list != NULL) {
            for (hash_node_t *node = buck->list; node->next != NULL; node = node->next) {

            }
        }
    }
}

void hashtable_insert(hashtable_t *this, const char *key, const char *value, uint32_t size) {

}

void hashtable_remove(hashtable_t *this, const char *key) {

}

void hashtable_rehash(hashtable_t *this) {

}

uint32_t _jhash(const char *key) {

}
