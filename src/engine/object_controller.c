//? septumfunk 2024
#include "object_controller.h"
#include "../data/fs.h"
#include "../data/stringext.h"
#include "../util/log.h"
#include "../win32/files.h"
#include "controller.h"
#include "sprite_manager.h"
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
    object_controller.state = luaL_newstate();
    luaL_openlibs(object_controller.state);
    luaL_dostring(object_controller.state, "package.path = package.path .. ';assets/scripts/?.lua'");
    object_controller_push_functions();
}

void object_controller_cleanup(void) {
    hashtable_delete(&object_controller.object_table);
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
            if (lua_isfunction(object_controller.state, -1)) {
                // Push ID
                lua_pushinteger(object_controller.state, object->id);

                // Call update(id)
                if (lua_pcall(object_controller.state, 1, 0, 0) != 0) {
                    log_error("in objects \"%s\" update logic: %s", object->type, lua_tostring(object_controller.state, -1));
                    lua_pop(object_controller.state, 1);
                }
            } else {
                log_error("'update' is not a function in object \"%s\"", object->type);
                lua_pop(object_controller.state, 1);
            }
        }
    }
}

void object_controller_draw(void) {
    uint32_t count;
    pair_t **pairs = hashtable_pairs(&object_controller.object_table, &count);
    gameobject_t *new_list = NULL;

    for (uint32_t i = 0; i < count; ++i) {
        if (pairs[i]->value == NULL)
            continue;

        // Reel back
        while (((gameobject_t *)pairs[i]->value)->previous != NULL)
            pairs[i]->value =((gameobject_t *)pairs[i]->value)->previous;

        for (gameobject_t *obj = pairs[i]->value; obj != NULL; obj = obj->next) {
            if (!new_list) {
                new_list = obj;
                continue;
            }

            gameobject_t *object = new_list;
            while (true) {
                if (obj->id > ->id) {
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
        }
    }
}

result_t object_controller_new(const char *name) {
    result_t res = no_error();
    char *update_p = format("assets/objects/%s/%s.lua", name, name);

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
        goto cleanup;
    }

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

void object_controller_push_functions(void) {
    // lua_object
    lua_pushcfunction(object_controller.state, lua_object);
    lua_setglobal(object_controller.state, "object");
    // lua_sprite_manager_draw
    lua_pushcfunction(object_controller.state, lua_draw_sprite);
    lua_setglobal(object_controller.state, "draw_sprite");
    // lua_is_key_pressed
    lua_pushcfunction(object_controller.state, lua_is_key_pressed);
    lua_setglobal(object_controller.state, "is_key_pressed");
    // lua_is_key_down
    lua_pushcfunction(object_controller.state, lua_is_key_down);
    lua_setglobal(object_controller.state, "is_key_down");
}

void object_controller_get_field(uint32_t id, const char *name) {
    lua_getglobal(object_controller.state, "objects");
    if (lua_isnil(object_controller.state, -1)) {
        lua_pop(object_controller.state, 1);
        lua_newtable(object_controller.state);
        lua_setglobal(object_controller.state, "objects");
        lua_getglobal(object_controller.state, "objects");
    }

    lua_pushinteger(object_controller.state, id);
    lua_gettable(object_controller.state, -2);
    if (lua_istable(object_controller.state, -1)) {
        lua_getfield(object_controller.state, -1, name);
    } else
        log_error("couldn't find object '%u' in objects list", id);
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

    if (!lua_istable(object_controller.state, -1))
        lua_pushnil(object_controller.state);

    return 1;
}