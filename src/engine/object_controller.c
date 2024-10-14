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
#include <stdlib.h>
#include <string.h>

object_controller_t object_controller;

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
}

void object_controller_cleanup(void) {
    uint32_t count = 0;
    pair_t **pairs = hashtable_pairs(&object_controller.object_table, &count);
    for (uint32_t i = 0; i < count; ++i) {
        gameobject_t *list = pairs[i]->value;
        if (list == NULL)
            continue;
        while (list->previous != NULL)
            list = list->previous;
        while (list != NULL) {
            gameobject_t *l = list;
            list = list->next;
            free(l);
        }
    }
    hashtable_delete(&object_controller.object_table);
    lua_close(object_controller.state);
}

void object_controller_update(void) {
    object_controller_update_globals();
    uint32_t count = 0;
    pair_t **pairs = hashtable_pairs(&object_controller.object_table, &count);
    if (!pairs)
        return;

    for (uint32_t i = 0; i < count; ++i) {
        gameobject_t *list = pairs[i]->value;
        // Reel back to the start
        while (list->previous != NULL)
            list = list->previous;

        gameobject_t *next; // Used because objects may delete themselves.
        for (gameobject_t *object = list; object != NULL; object = next) {
            next = object->next;
            object_controller_get_field(object->id, "update");
            if (!lua_isfunction(object_controller.state, -1)) {
                lua_pop(object_controller.state, 1);
                continue;
            }
            object_controller_get(object->id);
            if (!lua_istable(object_controller.state, -1)) {
                lua_pop(object_controller.state, 1);
                log_error("Unable to locate object '%s' instance %u", object->type, object->id);
                continue;
            }

            if (lua_pcall(object_controller.state, 1, 0, 0) != 0) {
                log_error("Object '%s' instance %u: %s", object->type, object->id, lua_tostring(object_controller.state, -1));
                lua_pop(object_controller.state, 1);
            }
        }
    }
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
    lua_pushnumber(object_controller.state, window.dimensions[0]);
    lua_settable(object_controller.state, -3);
    lua_pushstring(object_controller.state, "height");
    lua_pushnumber(object_controller.state, window.dimensions[1]);
    lua_settable(object_controller.state, -3);
    lua_pop(object_controller.state, 1);
}

void object_controller_draw(void) {
    uint32_t count;
    pair_t **pairs = hashtable_pairs(&object_controller.object_table, &count);
    gameobject_t *draw_list = NULL;

    for (uint32_t i = 0; i < count; ++i) {
        if (pairs[i]->value == NULL)
            continue;

        // Reel back
        while (((gameobject_t *)pairs[i]->value)->previous != NULL)
            pairs[i]->value =((gameobject_t *)pairs[i]->value)->previous;

        for (gameobject_t *obj = pairs[i]->value; obj != NULL; obj = obj->next) {
            // Does draw or draw gui exist?
            object_controller_get_field(obj->id, "draw");
            object_controller_get_field(obj->id, "draw_gui");
            if (!lua_isfunction(object_controller.state, -1) && !lua_isfunction(object_controller.state, -2)) {
                lua_pop(object_controller.state, 2);
                continue;
            }
            lua_pop(object_controller.state, 2);

            // Get depth
            float depth = 0.0f;
            object_controller_get_field(obj->id, "depth");
            if (lua_isnumber(object_controller.state, -1))
                depth = lua_tonumber(object_controller.state, -1);
            lua_pop(object_controller.state, 1);

            // Make draw call version
            gameobject_t *draw_call = calloc(1, sizeof(gameobject_t));
            *draw_call = (gameobject_t) {
                obj->id,
                obj->type,
                depth,
                NULL, NULL,
            };

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
    }

    // Reel back
    while (draw_list != NULL && draw_list->previous != NULL)
        draw_list = draw_list->previous;

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
            log_error("In object '%s' instance %u: %s", object->type, object->id, lua_tostring(object_controller.state, -1));
            lua_pop(object_controller.state, 1);
        }
    }
    // Draw GUI Calls
    renderer.gui = true;
    renderer_update_buffer();
    for (gameobject_t *object = draw_list; object != NULL; object = object->next) {
        if (object->previous)
            free(object->previous);
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
            log_error("In object '%s' instance %u: %s", object->type, object->id, lua_tostring(object_controller.state, -1));
            lua_pop(object_controller.state, 1);
        }
    }
    renderer.gui = false;
    renderer_update_buffer();
}

result_t object_controller_new(const char *name, float x, float y) {
    result_t res = no_error();

    const char *true_name = name + strlen(name);
    while (*(true_name - 1) != '/' && true_name != name)
        true_name--;

    char *update_p = format("resources/objects/%s/%s.lua", name, true_name);

    if (!fs_exists(update_p)) {
        res =  error("ObjectNotFoundError", "The requested object could not be loaded because it could not be found.");
        goto cleanup;
    }

    gameobject_t *object = calloc(1, sizeof(gameobject_t));

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

    object->id = object_controller.current_id++;
    object->type = calloc(1, strlen(name) + 1);
    strcpy(object->type, name);

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

    gameobject_t *existing = hashtable_get(&object_controller.object_table, (void *)name);
    if (existing) {
        // Reel back
        while (existing->previous != NULL)
            existing = existing->previous;
        existing->previous = object;
        object->next = existing;
    } else
        hashtable_insert(&object_controller.object_table, (void *)name, object, sizeof(gameobject_t));

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
        log_error("Couldn't find object '%u' in objects list", id);
    }
}

int lua_object_get(lua_State *L) {
    object_controller_get(luaL_checkinteger(L, 1));
    return 1;
}

int lua_object_get_all(lua_State *L) {
    lua_newtable(L);
    int cnt = 1;
    for (
        gameobject_t *object = hashtable_get(&object_controller.object_table, (void *)luaL_checkstring(L, 1));
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
    uint32_t id = luaL_checkinteger(L, 1);
    object_controller_get(id);
    if (!lua_istable(L, -1)) {
        lua_pop(L, 1);
        return 0;
    }
    lua_getfield(L, -1, "type");
    lua_remove(L, -2);
    if (!lua_isstring(L, -1)) {
        lua_pop(L, 1);
        return 0;
    }
    const char *type = lua_tostring(L, -1);
    lua_pop(L, 1);

    gameobject_t *ls = hashtable_get(&object_controller.object_table, (void *)type);
    // Reel back
    while (ls != NULL && ls->previous != NULL)
        ls = ls->previous;

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

            lua_getglobal(L, "objects");
            lua_pushinteger(L, id);
            lua_pushnil(L);
            lua_settable(L, -3);
            lua_pop(L, 1);
            break;
        }
    }
    return 0;
}
