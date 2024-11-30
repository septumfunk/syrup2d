#include "scene.h"
#include "../resources/fs.h"
#include "../resources/resource_manager.h"
#include "../scripting/scripting_api.h"
#include "../util/stringext.h"
#include <GLFW/glfw3.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

uint32_t instances_count(instance_t *instance) {
    uint32_t c = 0;
    while (instance) {
        ++c;
        instance = instance->next;
    }
    return c;
}

uint32_t instances_flattened_size(instance_t *instance) {
    instance_t *head = instance = instance_seek_start(instance);
    uint32_t size = sizeof(instance_header_t); // Header
    while (head) {
        size += strlen(head->type) + 1;
        size += sizeof(float) * 3;
        if (head->creation_code)
            size += strlen(head->creation_code) + 1;
        head = head->next;
    }
    return size;
}

void instances_flatten(instance_t *instance, char *buffer) {
    instance_t *head = instance = instance_seek_start(instance);

    *(instance_header_t *)buffer = (instance_header_t) {
        .version = INSTANCES_VERSION,
        .count = instances_count(instance),
    };
    buffer += sizeof(instance_header_t);

    while (head) {
        strcpy(buffer, head->type);
        buffer += strlen(head->type) + 1;

        ((float *)buffer)[0] = head->x;
        ((float *)buffer)[1] = head->y;
        ((float *)buffer)[2] = head->z;
        buffer += sizeof(float) * 3;

        strcpy(buffer, head->creation_code);
        buffer += strlen(head->creation_code) + 1;

        head = head->next;
    }
}

void instance_shift(instance_t *instance, int by) {
    if (instance == NULL || by == 0) {
        return;
    }

    if (by > 0) { // down
        for (int i = 0; i < by; ++i) {
            instance_t *next = instance->next;
            if (next == NULL) break;

            instance_t *previous = instance->previous;
            next->previous = previous;
            if (previous) previous->next = next;

            instance->previous = next;
            instance->next = next->next;
            if (next->next) next->next->previous = instance;
            next->next = instance;
        }
    } else { // up
        by = -by;
        for (int i = 0; i < by; ++i) {
            instance_t *previous = instance->previous;
            if (previous == NULL) break;

            instance_t *next = instance->next;
            previous->next = next;
            if (next) next->previous = previous;

            instance->next = previous;
            instance->previous = previous->previous;
            if (previous->previous) previous->previous->next = instance;

            previous->previous = instance;
        }
    }
}


instance_t *instance_seek_start(instance_t *instance) {
    while (instance && instance->previous)
        instance = instance->previous;
    return instance;
}

instance_t *instance_seek_end(instance_t *instance) {
    while (instance && instance->next)
        instance = instance->next;
    return instance;
}

scene_t scene_create(const char *name) {
    return (scene_t) {
        .name = _strdup(name),
        .instances = NULL,

        .current = false,
        .active = true,
        .last_accessed = glfwGetTime(),
    };
}

void scene_delete(scene_t *scene) {
    instance_t *head = scene->instances = instance_seek_start(scene->instances);
    while (head) {
        scene->instances = head->next;
        free(head);
        head = scene->instances;
    }
}

result_t scene_load(const char *name, scene_t *out) {
    char *path = format(SCENE_PATH, resource_manager.folder, name);
    if (!fs_exists(path))
        return result_error("SceneNotFoundErr", "Could not locate scene '%s'.", name);

    char *buffer = NULL;
    fs_size_t size;
    result_t res = fs_load_checksum(path, &buffer, &size);
    char *head = buffer;
    if (res.is_error)
        return res;

    out->name = _strdup(head);
    out->instances = NULL;
    head += strlen(name) + 1;

    // Instances
    instance_header_t *header = (instance_header_t *)head;
    head += sizeof(instance_header_t);

    if (header->version != INSTANCES_VERSION) {
        free(buffer);
        return result_error("SceneWrongVersionErr", "Scene '%s' is of incompatible version '%f'.", name, header->version);
    }

    for (uint32_t i = 0; i < header->count; ++i) {
        const char *name = head;
        head += strlen(name) + 1;

        float *coords = (float *)head;
        head += sizeof(float) * 3;

        const char *creation_code = head;
        head += strlen(creation_code) + 1;

        scene_add_instance(out, name, coords[0], coords[1], coords[2], creation_code);
    }

    return result_no_error();
}

result_t scene_save(scene_t *this, const char *name) {
    int size = strlen(this->name) + 1 + instances_flattened_size(this->instances);
    char *buffer, *head;
    head = buffer = calloc(1, size);

    strcpy(head, this->name);
    head += strlen(this->name) + 1;

    instances_flatten(this->instances, head);

    char *path = format(SCENE_PATH, resource_manager.folder, name);
    result_t res = fs_save_checksum(path, buffer, size);

    free(path);
    return res;
}

void scene_spawn_instances(scene_t *this) {
    instance_t *head = this->instances = instance_seek_start(this->instances);
    while (head) {
        // TODO: Creation Code and z in scripting_api_push
        result_t res = scripting_api_push(head->type, head->x, head->y, head->z, true, head->creation_code);
        panic(res);
        head = head->next;
    }
}

void scene_add_instance(scene_t *this, const char *name, float x, float y, float z, const char *creation_code) {
    instance_t *inst = calloc(1, sizeof(instance_t));
    *inst = (instance_t) {
        .type = _strdup(name),
        .x = x,
        .y = y,
        .z = z,
        .creation_code = creation_code ? _strdup(creation_code) : "",
    };

    if (!this->instances) this->instances = inst;
    else instance_seek_end(this->instances)->next = inst;
}

void scene_delete_instance(scene_t *this, uint32_t index) {
    this->instances = instance_seek_start(this->instances);

    for (uint32_t i = 0; i < index; ++i) {
        if (!this->instances)
            return;
        this->instances = this->instances->next;
    }

    this->instances->previous->next = this->instances->next;
    this->instances->next->previous = this->instances->previous;

    instance_t *del = this->instances;
    this->instances = instance_seek_start(this->instances);
    free(del);
}

void scene_set_instance_type(scene_t *this, uint32_t index, const char *type) {
    this->instances = instance_seek_start(this->instances);
    for (uint32_t i = 0; i < index; ++i) {
        if (!this->instances)
            return;
        this->instances = this->instances->next;
    }

    free(this->instances->type);
    this->instances->type = _strdup(type);

    this->instances = instance_seek_start(this->instances);
}

void scene_move_instance(scene_t *this, uint32_t index, float x, float y, float z) {
    this->instances = instance_seek_start(this->instances);
    for (uint32_t i = 0; i < index; ++i) {
        if (!this->instances)
            return;
        this->instances = this->instances->next;
    }

    this->instances->x = x;
    this->instances->y = y;
    this->instances->z = z;

    this->instances = instance_seek_start(this->instances);
}
