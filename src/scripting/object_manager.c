#include "object_manager.h"
#include "../util/crypto.h"
#include "../util/stringext.h"
#include "scripting_api.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void object_list_insert(object_list_t *list, object_t object) {
    object_t *new = malloc(sizeof(object_t));
    memcpy(new, &object, sizeof(object_t));
    new->next = list->start;
    list->start = new;
}

void object_list_remove(object_list_t *list, uint32_t id) {
    object_t *previous = NULL;
    for (object_t *object = list->start; object != NULL; object = object->next) {
        if (object->id == id) {
            if (previous)
                previous->next = object->next;
            else
                list->start = object->next;
            free(object);
            return;
        }
        previous = object;
    }
}

void object_list_sort(object_list_t *list) {
    object_t *new_start = NULL;
    for (object_t *o_index = list->start; o_index != NULL; o_index = o_index->next) {
        object_t *object = calloc(1, sizeof(object_t));
        object->id = o_index->id;
        object->depth = o_index->depth;

        if (!new_start) {
            new_start = object;
            continue;
        }

        if (object->depth > new_start->depth || (object->depth == new_start->depth && new_start->id > object->id)) {
            object->next = new_start;
            new_start = object;
            continue;
        }

        object_t *head = new_start;
        while (true) {
            if (head->next) {
                if (object->depth < head->next->depth) { // Traverse rightward
                    head = head->next;
                    continue;
                }

                if (head->next->depth == object->depth) { // Sort by ID
                    while (head->next && head->next->id > object->id && object->next && object->next->depth == object->depth)
                        head = head->next;
                    object->next = head->next;
                    head->next = object;
                    break;
                }

                // Insert
                object->next = head->next;
                head->next = object;
                break;
            }
            head->next = object;
            break;
        }
    }
    object_list_t ls_del = { .type = NULL, .start = list->start };
    object_list_delete(&ls_del);
    list->start = new_start;
}

void object_list_delete(object_list_t *list) {
    object_t *next = NULL;
    while (list->start) {
        next = list->start->next;
        free(list->start);
        list->start = next;
    }
    free(list->type);
}

object_manager_t object_manager_create(void) {
    return (object_manager_t) {
        .bucket_count = 16,
        .list_count = 0,
        .buckets = calloc(16, sizeof(object_bucket_t)),
        .load = 0,
    };
}

void object_manager_delete(object_manager_t *this) {
    for (object_bucket_t *bucket = this->buckets; bucket < this->buckets + this->bucket_count; ++bucket) {
        for (object_list_t *list = bucket->start; list != NULL; list = list->next) {
            object_list_delete(list);
            free(list);
            object_manager_rehash(this);
        }
    }
    free(this->buckets);
}

void object_manager_push(object_manager_t *this, const char *type, uint32_t id, float depth) {
    object_list_t *list = object_manager_type(this, type);
    if (!list) { // Insert list
        list = calloc(1, sizeof(object_list_t));
        list->type = strdup(type);
        object_bucket_t *bucket = &this->buckets[jhash_str(type) & (this->bucket_count - 1)];
        list->next = bucket->start;
        bucket->start = list;
        this->list_count++;
        object_manager_rehash(this);
    }
    object_list_insert(list, (object_t) { .id = id, .depth = depth });
    object_list_sort(list);
}

object_t *object_manager_get(object_manager_t *this, uint32_t id) {
    for (object_bucket_t *bucket = this->buckets; bucket < this->buckets + this->bucket_count; ++bucket) {
        for (object_list_t *list = bucket->start; list != NULL; list = list->next) {
            for (object_t *object = list->start; object != NULL; object = object->next) {
                if (object->id == id)
                    return object;
            }
        }
    }
    return NULL;
}

object_t *object_manager_get_all(object_manager_t *this) {
    object_t *new_objects = NULL;

    for (object_bucket_t *bucket = this->buckets; bucket < this->buckets + this->bucket_count; ++bucket) {
        for (object_list_t *list = bucket->start; list != NULL; list = list->next) {
            for (object_t *object = list->start; object != NULL; object = object->next) {
                object_t *cpy = calloc(1, sizeof(object_t));
                memcpy(cpy, object, sizeof(object_t));
                cpy->next = new_objects;
                new_objects = cpy;
            }
        }
    }

    object_list_t list = (object_list_t) { .start = new_objects };
    object_list_sort(&list);

    return list.start;
}

void object_manager_remove(object_manager_t *this, uint32_t id) {
    for (object_bucket_t *bucket = this->buckets; bucket < this->buckets + this->bucket_count; ++bucket) {
        object_list_t *prev_list = NULL;
        for (object_list_t *list = bucket->start; list != NULL; list = list->next) {
            for (object_t *object = list->start; object != NULL; object = object->next) {
                if (object->id == id) {
                    object_list_remove(list, id);
                    if (!list->start) {
                        if (list == bucket->start)
                            bucket->start = list->next;
                        else prev_list->next = list->next;

                        object_list_delete(list);
                        free(list);
                        this->list_count--;
                        object_manager_rehash(this);
                    }
                    return;
                }
            }
            prev_list = list;
        }
    }
}

object_list_t *object_manager_type(object_manager_t *this, const char *type) {
    object_list_t *list = this->buckets[jhash_str(type) & (this->bucket_count - 1)].start;
    object_list_t *out = NULL;
    for (object_list_t *l = list; l; l = l->next) {
        if (bstrcmp(l->type, type)) {
            out = l;
            break;
        }
    }
    return out;
}

void object_manager_remove_list(object_manager_t *this, const char *type) {
    object_list_t *list = this->buckets[jhash_str(type) & (this->bucket_count - 1)].start;

    object_list_t *out = NULL;
    object_list_t *prev = NULL;
    while (list && !bstrcmp(list->type, type) && list->next) {
        prev = list;
        list = list->next;
    }
    if (out) {
        if (prev) prev->next = list->next;
        free(out);
    }
}

void object_manager_rehash(object_manager_t *this) {
    float load_before = this->load;
    this->load = this->list_count / this->bucket_count;

    uint32_t bucket_count = this->bucket_count;
    if (this->load < 0.7 && load_before >= 0.7)
        bucket_count /= 2;
    else if (this->load > 0.7)
        bucket_count *= 2;

    if (bucket_count == this->bucket_count)
        return;

    object_bucket_t *old_buckets = this->buckets;
    this->buckets = calloc(bucket_count, sizeof(object_bucket_t));

    // Rehash process
    for (object_bucket_t *bucket = old_buckets; bucket < old_buckets + this->bucket_count; ++bucket) {
        for (object_list_t *list = bucket->start; list != NULL; list = list->next) {
            object_bucket_t *bucket = this->buckets + jhash_str(list->type);
            list->next = bucket->start;
            bucket->start = list;
        }
    }

    this->bucket_count = bucket_count;
    this->load = this->list_count / this->bucket_count;
}