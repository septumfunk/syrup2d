#pragma once
#include "cursor.h"
#include "scene.h"
#include "sprite.h"
#include "../graphics/shader.h"
#include "../structures/hashtable.h"
#include <lua.h>
#include <stdint.h>

#define META_VERSION 0.3f
#define META_PATH "%s/meta.sym"
typedef struct syrup_meta_t {
    float version;
    char *title;
    uint16_t width, height;
    bool fixed_size;
    uint8_t window_scale;
} syrup_meta_t;

#define GARBAGE_COLLECTOR_ENABLED true
typedef struct resource_manager_t {
    bool gc;
    syrup_meta_t metadata;
    hashtable_t scene_table;
    hashtable_t sprite_table;
    hashtable_t cursor_table;

    char *folder;
    scene_t *current_scene;
} resource_manager_t;
extern resource_manager_t resource_manager;

void resource_manager_init(bool garbage_collector);
void resource_manager_cleanup(void);

result_t resource_manager_load_metadata(const char *folder);
void resource_manager_save_metadata(const char *folder);

scene_t *resource_manager_scene(const char *name);
void resource_manager_scene_goto(const char *name);

sprite_t *resource_manager_sprite(const char *name);
result_t resource_manager_import_sprite(const char *name, uint8_t frame_count, uint8_t frame_delay);

GLFWcursor *resource_manager_cursor(mouse_cursors_e cursor);

void resource_manager_clean(void);