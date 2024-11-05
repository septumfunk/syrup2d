#pragma once
#include <stdint.h>

typedef struct object_t {
    uint32_t id;
    float z;
    struct object_t *next;
} object_t;

typedef struct object_list_t {
    char *type;
    object_t *start;
    struct object_list_t *next;
} object_list_t;

void object_list_insert(object_list_t *list, object_t object);
void object_list_remove(object_list_t *list, uint32_t id);
void object_list_sort(object_list_t *list);
void object_list_delete(object_list_t *list);

typedef struct object_bucket_t {
    object_list_t *start;
} object_bucket_t;

typedef struct object_manager_t {
    uint32_t bucket_count, list_count;
    object_bucket_t *buckets;
    float load;
} object_manager_t;

object_manager_t object_manager_create(void);
void object_manager_delete(object_manager_t *this);

void object_manager_push(object_manager_t *this, const char *type, uint32_t id, float z);
object_t *object_manager_get(object_manager_t *this, uint32_t id);
object_t *object_manager_get_all(object_manager_t *this);
void object_manager_remove(object_manager_t *this, uint32_t id);
object_list_t *object_manager_type(object_manager_t *this, const char *type);
void object_manager_remove_list(object_manager_t *this, const char *type);
void object_manager_rehash(object_manager_t *this);
