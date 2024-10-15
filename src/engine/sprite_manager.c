//? septumfunk 2024
#include "sprite_manager.h"
#include "controller.h"
#include "engine.h"
#include "object_controller.h"
#include "../util/log.h"
#include "renderer.h"
#include <lauxlib.h>
#include <stdlib.h>
#include <string.h>

sprite_manager_t sprite_manager;

void sprite_manager_init(bool garbage_collecter) {
    memset(&sprite_manager, 0, sizeof(sprite_manager_t));
    sprite_manager.gc = garbage_collecter;
    sprite_manager.table = hashtable_string();

    renderer_bind("sprite");
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(4 * sizeof(float)));

    // Lua
    lua_pushcfunction(object_controller.state, lua_draw_sprite);
    lua_setglobal(object_controller.state, "draw_sprite");
    lua_pushcfunction(object_controller.state, lua_draw_text);
    lua_setglobal(object_controller.state, "draw_text");

    lua_pushcfunction(object_controller.state, lua_sprite_width);
    lua_setglobal(object_controller.state, "sprite_width");
    lua_pushcfunction(object_controller.state, lua_sprite_height);
    lua_setglobal(object_controller.state, "sprite_height");
}

void sprite_manager_cleanup(void) {
    hashtable_delete(&sprite_manager.table);
}

sprite_t *sprite_manager_get(const char *name) {
    sprite_t *spr;
    if ((spr = hashtable_get(&sprite_manager.table, (void *)name)) == NULL) {
        spr = calloc(1, sizeof(sprite_t));
        // Attempt to load sprite
        if (sprite_load(spr, name).is_error) {
            log_info("Failed to load sprite '%s'. Loading default sprite.", name);
            if (sprite_load(spr, "default").is_error)
                panic(error("Fatal Error", "Sprite manager is unable to load default sprite. Are your assets corrupt?"));
        } else
            log_info("Loaded sprite '%s'.", name);

        sprite_t *pspr = hashtable_insert(&sprite_manager.table, (void *)name, spr, sizeof(sprite_t));
        free(spr);
        spr = pspr;
    }
    spr->ref_count++;
    return spr;
}

void sprite_manager_import(const char *name, uint8_t frame_count, uint8_t frame_delay) {
    sprite_t spr;
    result_t res;
    if ((res = sprite_from_image(&spr, name)).is_error) {
        error_warn(res);
        return;
    }

    spr.data.frame_count = frame_count;
    spr.data.frame_delay = frame_delay;

    if (hashtable_get(&sprite_manager.table, (void *)name))
        hashtable_remove(&sprite_manager.table, (void *)name);
    hashtable_insert(&sprite_manager.table, (void *)name, &spr, sizeof(sprite_t));

    sprite_save(&spr);
}

void sprite_manager_draw(const char *name, float x, float y, uint8_t frame_index) {
    sprite_draw(sprite_manager_get(name), x, y, frame_index);
}

int lua_draw_sprite(lua_State *L) {
    const char *name = luaL_checkstring(L, 1);
    double x = luaL_checknumber(L, 2);
    double y = luaL_checknumber(L, 3);
    int frame_index = luaL_checkinteger(L, 4);
    sprite_manager_draw(name, x, y, frame_index);
    return 0;
}

void sprite_manager_draw_tint(const char *name, float x, float y, uint8_t frame_index, color_t color) {
    sprite_draw_tint(sprite_manager_get(name), x, y, frame_index, color);
}

int lua_draw_sprite_tint(lua_State *L) {
    const char *name = luaL_checkstring(L, 1);
    double x = luaL_checknumber(L, 2);
    double y = luaL_checknumber(L, 3);
    int frame_index = luaL_checkinteger(L, 4);

    // Color
    luaL_checktype(L, 5, LUA_TTABLE);
    lua_getfield(L, 5, "r");
    float r = luaL_checknumber(L, -1);
    lua_pop(L, 1);
    lua_getfield(L, 5, "g");
    float g = luaL_checknumber(L, -1);
    lua_pop(L, 1);
    lua_getfield(L, 5, "b");
    float b = luaL_checknumber(L, -1);
    lua_pop(L, 1);
    lua_getfield(L, 5, "a");
    float a = luaL_checknumber(L, -1);
    lua_pop(L, 1);

    sprite_manager_draw_tint(name, x, y, frame_index, (color_t){ r, g, b, a });
    return 0;
}

void sprite_manager_draw_text(const char *name, float x, float y, const char *text, color_t color) {
    sprite_t *spr = sprite_manager_get(name);
    int charwidth = spr->data.width / spr->data.frame_count;
    float x_offset = 0;

    for (const char *c = text; *c != '\0'; ++c) {
        char idx = *c;

        if (idx == '\n') {
            y += spr->data.height;
            x_offset = 0;
            continue;
        }

        if (idx > 126 || idx < 32)
            idx = '?';

        sprite_manager_draw_tint(name, x + x_offset, y, idx - 32, color);
        x_offset += charwidth;
    }
}

int lua_draw_text(lua_State *L) {
    const char *sprite = luaL_checkstring(L, 1);
    float x = luaL_checknumber(L, 2);
    float y = luaL_checknumber(L, 3);
    const char *text = luaL_checkstring(L, 4);

    // Color
    luaL_checktype(L, 5, LUA_TTABLE);
    lua_getfield(L, 5, "r");
    float r = luaL_checknumber(L, -1);
    lua_pop(L, 1);
    lua_getfield(L, 5, "g");
    float g = luaL_checknumber(L, -1);
    lua_pop(L, 1);
    lua_getfield(L, 5, "b");
    float b = luaL_checknumber(L, -1);
    lua_pop(L, 1);
    lua_getfield(L, 5, "a");
    float a = luaL_checknumber(L, -1);
    lua_pop(L, 1);

    sprite_manager_draw_text(sprite, x, y, text, (color_t) { r, g, b, a });
    return 0;
}

void sprite_manager_clean(void) {
    if (!sprite_manager.gc || sprite_manager.table.pair_count == 0)
        return;
    uint32_t count = 0;
    pair_t **pairs = hashtable_pairs(&sprite_manager.table, &count);
    for (pair_t **pair = pairs; pair < pairs + count; ++pair) {
        sprite_t *spr = (sprite_t *)(*pair)->value;
        if (spr->ref_count == 0) {
            spr->decay -= engine.delta_time;
            if (spr->decay <= 0) {
                sprite_delete(spr);
                log_info("Unloaded sprite '%s'.", (*pair)->key);
                hashtable_remove(&sprite_manager.table, (*pair)->key);
            }
        } else {
            spr->decay = SPRITE_DECAY_TIME;
            spr->ref_count = 0;
        }
    }
    free(pairs);
}

int lua_sprite_width(lua_State *L) {
    const char *name = luaL_checkstring(L, 1);
    sprite_t *spr = sprite_manager_get(name);
    lua_pushinteger(L, spr->data.width);
    return 1;
}

int lua_sprite_height(lua_State *L) {
    const char *name = luaL_checkstring(L, 1);
    sprite_t *spr = sprite_manager_get(name);
    lua_pushinteger(L, spr->data.height);
    return 1;
}
