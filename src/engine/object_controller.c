//? septumfunk 2024
#include "object_controller.h"
#include "../data/fs.h"
#include "../data/stringext.h"
#include "../util/log.h"
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
    luaL_dostring(object_controller.state, "package.path = package.path .. ';resources/scripts/?.lua'");
}

void object_controller_cleanup(void) {
    hashtable_delete(&object_controller.object_table);
    lua_close(object_controller.state);
}

void object_controller_update(void) {
    uint32_t count = 0;
    pair_t **pairs = hashtable_pairs(&object_controller.object_table, &count);
    if (!pairs)
        return;

    for (uint32_t i = 0; i < count; ++i) {
        gameobject_t *list = pairs[i]->value;
        // Reel back to the start
        while (list->previous != NULL)
            list = list->previous;

        for (gameobject_t *object = list; object != NULL; object = object->next) {
            object_controller_get_field(object->id, "update");
            if (!lua_isfunction(object_controller.state, -1)) {
                log_error("'update' is not a function in object '%s' instance %u", object->type, object->id);
                continue;
            }
            object_controller_get(object->id);
            if (!lua_istable(object_controller.state, -1)) {
                log_error("Unable to locate object '%s' instance %u", object->type, object->id);
                continue;
            }

            if (lua_pcall(object_controller.state, 1, 0, 0) != 0)
                log_error("Object '%s' instance %u: %s", object->type, object->id, lua_tostring(object_controller.state, -1));
        }
    }
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
            // Does draw exist?
            object_controller_get_field(obj->id, "draw");
            if (!lua_isfunction(object_controller.state, -1))
                continue;

            // Get depth
            float depth = 0.0f;
            object_controller_get_field(obj->id, "depth");
            if (lua_isnumber(object_controller.state, -1))
                depth = lua_tonumber(object_controller.state, -1);

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
                        draw_list = draw_list->next;
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
    while (draw_list->previous != NULL)
        draw_list = draw_list->previous;

    for (gameobject_t *object = draw_list; object != NULL; object = object->next) {
        if (object->previous)
            free(object->previous);

        object_controller_get_field(object->id, "draw");
        if (!lua_isfunction(object_controller.state, -1)) {
            log_error("'draw' is not a function in object '%s' instance %u", object->type, object->id);
            continue;
        }
        object_controller_get(object->id);
        if (!lua_istable(object_controller.state, -1)) {
            log_error("Unable to locate object '%s' instance %u", object->type, object->id);
            continue;
        }

        if (lua_pcall(object_controller.state, 1, 0, 0) != 0)
            log_error("In object '%s' instance %u: %s", object->type, object->id, lua_tostring(object_controller.state, -1));
    }
}

result_t object_controller_new(const char *name, float x, float y) {
    result_t res = no_error();
    char *update_p = format("resources/objects/%s/%s.lua", name, name);

    if (!fs_exists(update_p)) {
        res =  error("ObjectNotFoundError", "The requested object could not be loaded because it could not be found.");
        goto cleanup;
    }

    gameobject_t *object = calloc(1, sizeof(gameobject_t));

    if (luaL_dofile(object_controller.state, update_p)) {
        res = error("ObjectCodeError", lua_tostring(object_controller.state, -1));
        goto cleanup;
    }

    if (!lua_istable(object_controller.state, -1)) {
        res = error("InvalidObjectError", "Object did not return a table in its code file.");
        goto cleanup;
    }

    // Position
    lua_pushliteral(object_controller.state, "x");
    lua_pushnumber(object_controller.state, x);
    lua_settable(object_controller.state, -3);

    lua_pushliteral(object_controller.state, "y");
    lua_pushnumber(object_controller.state, x);
    lua_settable(object_controller.state, -3);

    // Objects table
    lua_getglobal(object_controller.state, "objects");
    if (lua_isnil(object_controller.state, -1)) {
        lua_pop(object_controller.state, 1);
        lua_newtable(object_controller.state);
        lua_setglobal(object_controller.state, "objects");
        lua_getglobal(object_controller.state, "objects");
    }
    lua_pushinteger(object_controller.state, object_controller.current_id);
    lua_pushvalue(object_controller.state, -3);
    lua_settable(object_controller.state, -3);

    // Id and Type
    object->id = object_controller.current_id++;
    object->type = calloc(1, strlen(name));
    strcpy(object->type, name);

    gameobject_t *existing = hashtable_get(&object_controller.object_table, (void *)name);
    if (existing) {
        while (true) {
            if (existing->id > object->id) {
                if (existing->previous) {
                    existing = existing->previous;
                    continue;
                }
                existing->previous = object;
            } else {
                if (existing->next) {
                    existing = existing->next;
                    continue;
                }
                existing->next = object;
            }
        }
    } else
        hashtable_insert(&object_controller.object_table, (void *)name, object, sizeof(gameobject_t));
cleanup:
    free(update_p);
    return res;
}

void object_controller_get(uint32_t id) {
    lua_getglobal(object_controller.state, "objects");
    if (lua_isnil(object_controller.state, -1)) {
        lua_pop(object_controller.state, 1);
        lua_newtable(object_controller.state);
        lua_setglobal(object_controller.state, "objects");
        lua_getglobal(object_controller.state, "objects");
    }

    lua_pushinteger(object_controller.state, id);
    lua_gettable(object_controller.state, -2);

    lua_remove(object_controller.state, -2);
    lua_remove(object_controller.state, -3);

    if (!lua_istable(object_controller.state, -1)) {
        log_error("Couldn't find object '%u' in objects list", id);
        lua_pop(object_controller.state, 1);
    }
}

void object_controller_get_field(uint32_t id, const char *name) {
    object_controller_get(id);
    if (lua_istable(object_controller.state, -1)) {
        lua_getfield(object_controller.state, -1, name);
    } else
        log_error("Couldn't find object '%u' in objects list", id);
}

int lua_object(lua_State *L) {
    const char *name = luaL_checkstring(L, 1);
    gameobject_t *other = hashtable_get(&object_controller.object_table, name);
    if (other == NULL) {
        lua_pushnil(L);
        return 1;
    }

    lua_getglobal(object_controller.state, "objects");
    if (lua_isnil(object_controller.state, -1)) {
        lua_pop(object_controller.state, 1);
        lua_newtable(object_controller.state);
        lua_setglobal(object_controller.state, "objects");
        lua_getglobal(object_controller.state, "objects");
    }
    lua_pushinteger(object_controller.state, other->id);
    lua_gettable(object_controller.state, -2);
    lua_pop(object_controller.state, -2);

    if (!lua_istable(object_controller.state, -1))
        lua_pushnil(object_controller.state);

    return 1;
}