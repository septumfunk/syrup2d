//? septumfunk 2024
#pragma once
#include "../data/hashtable.h"
#include "../graphics/sprite.h"
#include <lua.h>
#include <stdint.h>

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
/// Import a sprite from the import folder.
result_t sprite_manager_import(const char *name, uint8_t frame_count, uint8_t frame_delay);
int lua_import_sprite(lua_State *L);
/// Draw a sprite from the manager's table.
/// This function may load the sprite if it's not already loaded.
void sprite_manager_draw(const char *name, float x, float y, uint8_t frame_index);
int lua_draw_sprite(lua_State *L);
/// Draw a sprite from the manager's table with a color tint.
/// This function may load the sprite if it's not already loaded.
void sprite_manager_draw_tint(const char *name, float x, float y, uint8_t frame_index, color_t color);
int lua_draw_sprite_tint(lua_State *L);
/// Draw text using a sprite from the manager's table.
/// This function may load the sprite if it's not already loaded.
void sprite_manager_draw_text(const char *name, float x, float y, const char *text, color_t color);
int lua_draw_text(lua_State *L);
/// Perform cleaning actions like garbage collection. Call this at the end of the frame.
void sprite_manager_clean(void);

/// Access sprite width from lua.
int lua_sprite_width(lua_State *L);
/// Access sprite height from lua.
int lua_sprite_height(lua_State *L);
/// Access sprite frame count from lua.
int lua_sprite_frames(lua_State *L);