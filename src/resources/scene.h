#pragma once
#include "../structures/result.h"
#include <stdint.h>

/// An instance as a linked list node
#define INSTANCES_VERSION 0.1f
typedef struct instance_t {
    bool enabled;
    char *type;
    float x, y, z;
    char *creation_code;
    struct instance_t *previous, *next;
} instance_t;

#pragma pack(push, 1)
typedef struct instance_header_t {
    float version;
    uint32_t count;
} instance_header_t;
#pragma pack(pop)

uint32_t instances_count(instance_t *instance);
uint32_t instances_flattened_size(instance_t *instance);
void instances_flatten(instance_t *instance, char *buffer);
instance_t *instances_link(char *buffer);
void instance_shift(instance_t *instance, int by);
instance_t *instance_seek_start(instance_t *instance);
instance_t *instance_seek_end(instance_t *instance);

#define SCENE_PATH "%s/scenes/%s.syc"
#define SCENE_VERSION 0.1f
typedef struct scene_t {
    char *name;
    instance_t *instances;
} scene_t;

scene_t scene_create(const char *name);
void scene_delete(scene_t *scene);
result_t scene_load(const char *name, scene_t *out);
result_t scene_save(scene_t *this, const char *name);

void scene_spawn_instances(scene_t *this);
void scene_add_instance(scene_t *this, const char *name, float x, float y, float z, const char *creation_code);
void scene_delete_instance(scene_t *this, uint32_t index);
void scene_set_instance_type(scene_t *this, uint32_t index, const char *type);
void scene_move_instance(scene_t *this, uint32_t index, float x, float y, float z);