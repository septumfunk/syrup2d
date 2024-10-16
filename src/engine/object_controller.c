//? septumfunk 2024
#include "object_controller.h"
#include "../data/fs.h"
#include "../data/stringext.h"
#include "../util/log.h"
#include "engine.h"
#include "renderer.h"
#include "window.h"
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

object_controller_t object_controller;

gameobject_t *gameobject_create(uint32_t id, const char *type, float depth) {
    gameobject_t *object = calloc(1, sizeof(gameobject_t));
    *object = (gameobject_t) {
        .id = id,
        .type = type ? _strdup(type) : NULL,
        .depth = depth,
        .previous = NULL,
        .next = NULL,
    };
    return object;
}

void gameobject_delete(gameobject_t *this) {
    free(this->type);
    free(this);
}

gameobject_t *gameobject_reel_back(gameobject_t *list) {
    while (list && list->previous)
        list = list->previous;
    return list;
}

void object_controller_init(void) {
    // Table
    object_controller.current_id = 1;
    object_controller.object_table = hashtable_string();

    // Lua
    object_controller.state = luaL_newstate();
    luaL_openlibs(object_controller.state);

    char *p = format("package.path = package.path .. ';resources/scripts/?.lua'");
    luaL_dostring(object_controller.state, p);
    free(p);

    lua_pushcfunction(object_controller.state, lua_object_get);
    lua_setglobal(object_controller.state, "object_get");

    lua_pushcfunction(object_controller.state, lua_object_get_all);
    lua_setglobal(object_controller.state, "object_get_all");

    lua_pushcfunction(object_controller.state, lua_object_new);
    lua_setglobal(object_controller.state, "object_new");
    lua_pushcfunction(object_controller.state, lua_object_delete);
    lua_setglobal(object_controller.state, "object_delete");
    lua_pushcfunction(object_controller.state, lua_object_delete_all);
    lua_setglobal(object_controller.state, "object_delete_all");
}

void object_controller_cleanup(void) {
    object_controller_delete_all();
    hashtable_delete(&object_controller.object_table);
    lua_close(object_controller.state);
}

void object_controller_update(void) {
    object_controller_update_globals();
    uint32_t count = 0;
    pair_t **pairs = hashtable_pairs(&object_controller.object_table, &count);
    if (!pairs)
        return;

    uint32_t id_count = 0;
    uint32_t *ids = NULL;

    for (uint32_t i = 0; i < count; ++i) {
        gameobject_t *list = *(gameobject_t **)pairs[i]->value;
        if (!list)
            continue;
        list = gameobject_reel_back(list);
        for (gameobject_t *object = list; object != NULL; object = object->next) {
            ids = realloc(ids, (id_count + 1) * sizeof(uint32_t));
            ids[id_count] = object->id;
            id_count++;
        }
    }

    free(pairs);

    for (uint32_t *id = ids; id < ids + id_count; ++id) {
        object_controller_get_field(*id, "update");
        if (!lua_isfunction(object_controller.state, -1)) {
            lua_pop(object_controller.state, 1);
            continue;
        }
        object_controller_get(*id);
        if (!lua_istable(object_controller.state, -1)) {
            lua_pop(object_controller.state, 1);
            log_error("Unable to locate object instance %u", *id);
            continue;
        }

        if (lua_pcall(object_controller.state, 1, 0, 0) != 0) {
            log_error("Object %u: %s", *id, lua_tostring(object_controller.state, -1));
            lua_pop(object_controller.state, 1);
        }
    }

    free(ids);
}

void object_controller_update_globals(void) {
    lua_pushnumber(object_controller.state, engine.delta_time);
    lua_setglobal(object_controller.state, "delta_time");

    lua_getglobal(object_controller.state, "mouse");
    if (lua_isnil(object_controller.state, -1)) {
        lua_pop(object_controller.state, 1);
        lua_newtable(object_controller.state);
        lua_setglobal(object_controller.state, "mouse");
        lua_getglobal(object_controller.state, "mouse");
    }
    double x, y;
    renderer_fbo_mouse_position(&x, &y);
    lua_pushstring(object_controller.state, "x");
    lua_pushnumber(object_controller.state, x);
    lua_settable(object_controller.state, -3);
    lua_pushstring(object_controller.state, "y");
    lua_pushnumber(object_controller.state, y);
    lua_settable(object_controller.state, -3);
    lua_pop(object_controller.state, 1);

    lua_getglobal(object_controller.state, "dimensions");
    if (lua_isnil(object_controller.state, -1)) {
        lua_pop(object_controller.state, 1);
        lua_newtable(object_controller.state);
        lua_setglobal(object_controller.state, "dimensions");
        lua_getglobal(object_controller.state, "dimensions");
    }
    lua_pushstring(object_controller.state, "width");
    lua_pushnumber(object_controller.state, game_data.game_width);
    lua_settable(object_controller.state, -3);
    lua_pushstring(object_controller.state, "height");
    lua_pushnumber(object_controller.state, game_data.game_height);
    lua_settable(object_controller.state, -3);
    lua_pop(object_controller.state, 1);
}

void object_controller_draw(void) {
    uint32_t count;
    pair_t **pairs = hashtable_pairs(&object_controller.object_table, &count);
    gameobject_t *draw_list = NULL;

    uint32_t id_count = 0;
    uint32_t *ids = NULL;

    for (uint32_t i = 0; i < count; ++i) {
        gameobject_t *list = *(gameobject_t **)pairs[i]->value;
        if (!list)
            continue;
        list = gameobject_reel_back(list);
        for (gameobject_t *object = list; object != NULL; object = object->next) {
            ids = realloc(ids, (id_count + 1) * sizeof(uint32_t));
            ids[id_count] = object->id;
            id_count++;
        }
    }
    free(pairs);

    for (uint32_t *id = ids; id < ids + id_count; ++id) {
        // Does draw or draw gui exist?
        object_controller_get_field(*id, "draw");
        object_controller_get_field(*id, "draw_gui");
        if (!lua_isfunction(object_controller.state, -1) && !lua_isfunction(object_controller.state, -2)) {
            lua_pop(object_controller.state, 2);
            continue;
        }
        lua_pop(object_controller.state, 2);

        // Get depth
        float depth = 0.0f;
        object_controller_get_field(*id, "depth");
        if (lua_isnumber(object_controller.state, -1))
            depth = lua_tonumber(object_controller.state, -1);
        lua_pop(object_controller.state, 1);

        // Make draw call version
        gameobject_t *draw_call = gameobject_create(*id, NULL, depth);

        if (!draw_list) {
            draw_list = draw_call;
            continue;
        }

        while (true) {
            if (draw_call->depth > draw_list->depth) {
                if (draw_list->next) {
                    if (draw_list->next->depth > draw_call->depth) {
                        draw_list->next->previous = draw_call;
                        draw_call->next = draw_list->next;
                        draw_list->next = draw_call;
                        draw_call->previous = draw_list;
                        break;
                    }
                    draw_list = draw_list->next;
                    continue;
                }
                draw_list->next = draw_call;
                draw_call->previous = draw_list;
                break;
            } else {
                if (draw_list->previous) {
                    if (draw_list->previous->depth < draw_call->depth) {
                        draw_list->previous->next = draw_call;
                        draw_call->previous = draw_list->previous;
                        draw_list->previous = draw_call;
                        draw_call->next = draw_list;
                        break;
                    }
                    draw_list = draw_list->previous;
                    continue;
                }
                draw_list->previous = draw_call;
                draw_call->next = draw_list;
                break;
            }
        }
    }
    free(ids);

    draw_list = gameobject_reel_back(draw_list);

    // Draw calls
    for (gameobject_t *object = draw_list; object != NULL; object = object->next) {
        object_controller_get_field(object->id, "draw");
        if (!lua_isfunction(object_controller.state, -1)) {
            lua_pop(object_controller.state, 1);
            continue;
        }
        object_controller_get(object->id);
        if (!lua_istable(object_controller.state, -1)) {
            lua_pop(object_controller.state, 2);
            continue;
        }
        if (lua_pcall(object_controller.state, 1, 0, 0) != 0) {
            object_controller_get_field(object->id, "type");
            log_error("In object '%s' instance %u: %s", lua_tostring(object_controller.state, -1), object->id, lua_tostring(object_controller.state, -2));
            lua_pop(object_controller.state, 2);
        }
    }
    // Draw GUI Calls
    renderer.gui = true;
    renderer_update_buffer();

    gameobject_t *object = draw_list;
    while (object != NULL) {
        if (object->previous)
            gameobject_delete(object->previous);
        object_controller_get_field(object->id, "draw_gui");
        if (!lua_isfunction(object_controller.state, -1)) {
            lua_pop(object_controller.state, 1);
            continue;
        }
        object_controller_get(object->id);
        if (!lua_istable(object_controller.state, -1)) {
            lua_pop(object_controller.state, 1);
            continue;
        }
        if (lua_pcall(object_controller.state, 1, 0, 0) != 0) {
            object_controller_get_field(object->id, "type");
            log_error("In object '%s' instance %u: %s", lua_tostring(object_controller.state, -1), object->id, lua_tostring(object_controller.state, -2));
            lua_pop(object_controller.state, 2);
        }
        if (!object->next) {
            gameobject_delete(object);
            break;
        }
        object = object->next;
    }
    renderer.gui = false;
    renderer_update_buffer();
}

result_t object_controller_new(const char *name, float x, float y) {
    result_t res = no_error();
    char *update_p = format("resources/objects/%s.lua", name);

    if (!fs_exists(update_p)) {
        res =  error("ObjectNotFoundError", "The requested object could not be loaded because it could not be found.");
        goto cleanup;
    }

    if (luaL_dofile(object_controller.state, update_p)) {
        res = error("ObjectCodeError", lua_tostring(object_controller.state, -1));
        lua_pop(object_controller.state, 1);
        goto cleanup;
    }

    if (!lua_istable(object_controller.state, -1)) {
        res = error("InvalidObjectError", "Object did not return a table in its code file.");
        lua_pop(object_controller.state, 2);
        goto cleanup;
    }

    gameobject_t *object = gameobject_create(object_controller.current_id++, name, 0);

    // Position
    lua_pushstring(object_controller.state, "id");
    lua_pushnumber(object_controller.state, object->id);
    lua_settable(object_controller.state, -3);

    lua_pushstring(object_controller.state, "type");
    lua_pushstring(object_controller.state, object->type);
    lua_settable(object_controller.state, -3);

    lua_pushstring(object_controller.state, "x");
    lua_pushnumber(object_controller.state, x);
    lua_settable(object_controller.state, -3);

    lua_pushstring(object_controller.state, "y");
    lua_pushnumber(object_controller.state, y);
    lua_settable(object_controller.state, -3);

    lua_getfield(object_controller.state, -1, "depth");
    if (!lua_isnumber(object_controller.state, -1)) {
        lua_pushnumber(object_controller.state, object->depth);
        lua_setfield(object_controller.state, -3, "depth");
    }
    lua_pop(object_controller.state, 1);

    // Objects table
    lua_getglobal(object_controller.state, "objects");
    if (lua_isnil(object_controller.state, -1)) {
        lua_pop(object_controller.state, 1);
        lua_newtable(object_controller.state);
        lua_setglobal(object_controller.state, "objects");
        lua_getglobal(object_controller.state, "objects");
    }
    lua_pushinteger(object_controller.state, object->id);
    lua_pushvalue(object_controller.state, -3);
    lua_settable(object_controller.state, -3);
    lua_pop(object_controller.state, 2);

    gameobject_t **e_ptr = (gameobject_t **)hashtable_get(&object_controller.object_table, (void *)name);
    if (e_ptr) {
        gameobject_t *existing = *e_ptr;
        existing = gameobject_reel_back(existing);
        existing->previous = object;
        object->next = existing;
    } else
        hashtable_insert(&object_controller.object_table, (void *)name, &object, sizeof(gameobject_t *));

    // Call start
    object_controller_get_field(object->id, "start");
    if (!lua_isfunction(object_controller.state, -1)) {
        lua_pop(object_controller.state, 1);
        object_controller_get(object->id);
        goto cleanup;
    }
    object_controller_get(object->id);
    if (!lua_istable(object_controller.state, -1)) {
        lua_pop(object_controller.state, 1);
        object_controller_get(object->id);
        goto cleanup;
    }
    if (lua_pcall(object_controller.state, 1, 0, 0) != 0) {
        log_error("Object '%s' instance %u: %s", object->type, object->id, lua_tostring(object_controller.state, -1));
        lua_pop(object_controller.state, 1);
    }
    object_controller_get(object->id);
cleanup:
    free(update_p);
    return res;
}

void object_controller_delete(uint32_t id) {
    object_controller_get(id);
    if (!lua_istable(object_controller.state, -1)) {
        lua_pop(object_controller.state, 1);
        return;
    }

    lua_getfield(object_controller.state, -1, "type");
    lua_remove(object_controller.state, -3);
    if (!lua_isstring(object_controller.state, -1)) {
        lua_pop(object_controller.state, 1);
        return;
    }
    const char *type = lua_tostring(object_controller.state, -1);
    lua_pop(object_controller.state, 1);

    lua_getfield(object_controller.state, -1, "cleanup");
    if (lua_isfunction(object_controller.state, -1)) {
        object_controller_get(id);
        if (lua_pcall(object_controller.state, 1, 0, 0)) {
            log_error("In 'cleanup' of object '%s' instance %d: %s", type, id, lua_tostring(object_controller.state, 1));
            lua_pop(object_controller.state, 1);
        }
    }

    gameobject_t **ls_ptr = hashtable_get(&object_controller.object_table, (void *)type);
    gameobject_t *ls = NULL;
    if (!*ls_ptr)
        return;

    ls = *ls_ptr;

    if (ls->id == id) {
        hashtable_remove(&object_controller.object_table, (void *)type);
        lua_getglobal(object_controller.state, "objects");
        lua_pushinteger(object_controller.state, id);
        lua_pushnil(object_controller.state);
        lua_settable(object_controller.state, -3);
        lua_pop(object_controller.state, 1);
        if (ls->previous) {
            hashtable_insert(&object_controller.object_table, (void *)type, &(ls->previous), sizeof(gameobject_t *));
            ls->previous->next = ls->next;
        } else if (ls->next) {
            hashtable_insert(&object_controller.object_table, (void *)type, &(ls->next), sizeof(gameobject_t *));
            ls->next->previous = ls->previous;
        }
        gameobject_delete(ls);
        return;
    }

    ls = gameobject_reel_back(ls);

    for (
        gameobject_t *object = ls;
        object != NULL;
        object = object->next
    ) {
        if (object->id == id) {
            if (object->previous) object->previous->next = object->next;
            if (object->next) object->next->previous = object->previous;
            if (!object->previous && !object->next)
                hashtable_remove(&object_controller.object_table, (void *)type);

            lua_getglobal(object_controller.state, "objects");
            lua_pushinteger(object_controller.state, id);
            lua_pushnil(object_controller.state);
            lua_settable(object_controller.state, -3);
            lua_pop(object_controller.state, 1);
            gameobject_delete(object);
            break;
        }
    }
}

void object_controller_delete_all(void) {
    uint32_t count = 0;
    pair_t **pairs = hashtable_pairs(&object_controller.object_table, &count);

    uint32_t id_count = 0;
    uint32_t *ids = NULL;

    for (uint32_t i = 0; i < count; ++i) {
        if (!pairs[i]->value)
            continue;
        gameobject_t *list = *(gameobject_t **)pairs[i]->value;
        list = gameobject_reel_back(list);
        while (list) {
            ids = realloc(ids, (id_count + 1) * sizeof(uint32_t));
            ids[id_count] = list->id;
            id_count++;
            list = list->next;
        }
    }

    for (uint32_t i = 0; i < id_count; ++i)
        object_controller_delete(ids[i]);

    free(ids);
    free(pairs);
}

void object_controller_get(uint32_t id) {
    lua_getglobal(object_controller.state, "objects");
    if (!lua_istable(object_controller.state, -1)) {
        lua_pop(object_controller.state, 1);
        lua_newtable(object_controller.state);
        lua_setglobal(object_controller.state, "objects");
        lua_pushnil(object_controller.state);
        return;
    }

    lua_pushinteger(object_controller.state, id);
    lua_gettable(object_controller.state, -2);

    lua_remove(object_controller.state, -2);
}

void object_controller_get_field(uint32_t id, const char *name) {
    object_controller_get(id);
    if (lua_istable(object_controller.state, -1)) {
        lua_getfield(object_controller.state, -1, name);
        lua_remove(object_controller.state, -2);
    } else {
        lua_pop(object_controller.state, 1);
        lua_pushnil(object_controller.state);
    }
}

int lua_object_get(lua_State *L) {
    object_controller_get(luaL_checkinteger(L, 1));
    return 1;
}

int lua_object_get_all(lua_State *L) {
    lua_newtable(L);
    int cnt = 1;
    gameobject_t **o_ptr = hashtable_get(&object_controller.object_table, (void *)luaL_checkstring(L, 1));
    if (!o_ptr)
        return 1;

    gameobject_t *ls = *o_ptr;
    ls = gameobject_reel_back(ls);

    for (
        gameobject_t *object = ls;
        object != NULL;
        object = object->next
    ) {
        lua_pushinteger(L, cnt++);
        object_controller_get(object->id);
        lua_settable(L, -3);
    }
    return 1;
}

int lua_object_new(lua_State *L) {
    const char *name = luaL_checkstring(L, 1);
    result_t res = object_controller_new(name, luaL_checknumber(L, 2), luaL_checknumber(L, 3));
    if (res.is_error || !lua_istable(object_controller.state, -1)) {
        log_error("Failed to load object '%s'.", name);
        lua_pop(L, 1);
        lua_pushnil(L);
    }
    return 1;
}

int lua_object_delete(lua_State *L) {
    object_controller_delete(luaL_checkinteger(L, 1));
    return 0;
}

int lua_object_delete_all(unused lua_State *L) {
    object_controller_delete_all();
    return 0;
}