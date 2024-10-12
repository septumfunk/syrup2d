//? septumfunk 2024
#pragma once
#include "../data/hashtable.h"
#include "../graphics/sprite.h"
#include <lua.h>

#define GARBAGE_COLLECTOR_ENABLED true

/// Sprite management system. Loads sprites dynamically as used and (optionally) garbage collects them.
typedef struct sprite_manager_t {
    bool gc;
    hashtable_t table;
} sprite_manager_t;
extern sprite_manager_t sprite_manager;

/// Initialize sprite manager system.
void sprite_manager_init(bool garbage_collector);
/// Clean up after sprite manager system.
void sprite_manager_cleanup(void);

/// Get a sprite from the manager's table.
/// This function may load the sprite if it's not already loaded.
sprite_t *sprite_manager_get(const char *name);
/// Draw a sprite from the manager's table.
/// This function may load the sprite if it's not already loaded.
void sprite_manager_draw(const char *name, float x, float y);
int lua_draw_sprite(lua_State *L);
/// Perform cleaning actions like garbage collection. Call this at the end of the frame.
void sprite_manager_clean(void);

/// Access sprite width from lua.
int lua_sprite_width(lua_State *L);
/// Access sprite height from lua.
int lua_sprite_height(lua_State *L);