#include "scripting_api.h"
#include "object_manager.h"
#include "modules/modules.h"
#include "../resources/fs.h"
#include "../resources/resource_manager.h"
#include "../graphics/renderer.h"
#include "../util/log.h"
#include "../util/stringext.h"
#include <lauxlib.h>
#include <lua.h>
#include <stdint.h>
#include <string.h>

scripting_api_t scripting_api;

void scripting_api_init(void) {
    memset(&scripting_api, 0, sizeof(scripting_api_t));

    scripting_api_init_state();
    scripting_api_init_globals();
    scripting_api_init_modules();

    scripting_api.current_id = 1;
    scripting_api.manager = object_manager_create();
}

void scripting_api_init_state(void) {
    scripting_api.state = luaL_newstate();
    luaL_openlibs(scripting_api.state);
    luaL_dostring(
        scripting_api.state,
        "package.path = package.path .. ';resources/scripts/?.lua;resources/engine/scripts/?.lua;resources/scripts/?/init.lua;resources/engine/scripts/?/init.lua'"
    );
}

void scripting_api_init_globals(void) {
    lua_newtable(scripting_api.state);
    lua_setglobal(scripting_api.state, "syrup");
    lua_getglobal(scripting_api.state, "syrup");

    lua_newtable(scripting_api.state);
    lua_pushnumber(scripting_api.state, resource_manager.game_data.height);
    lua_setfield(scripting_api.state, -2, "width");
    lua_pushnumber(scripting_api.state, resource_manager.game_data.height);
    lua_setfield(scripting_api.state, -2, "height");
    lua_setfield(scripting_api.state, -2, "dimensions");

    lua_newtable(scripting_api.state);
    lua_setfield(scripting_api.state, -2, "objects");
    lua_newtable(scripting_api.state);
    lua_setfield(scripting_api.state, -2, "instances");

    scripting_api_update_globals();
}

void scripting_api_init_modules(void) {
    lua_getglobal(scripting_api.state, "syrup");
    for (scripting_module_t *module = scripting_modules; module < scripting_modules + SCRIPTING_MODULES_COUNT; ++module) {
        if (!module->name || !module->functions)
            continue;
        log_info("Registering module '%s'", module->name);
        lua_newtable(scripting_api.state);
        lua_setfield(scripting_api.state, -2, module->name);
        lua_getfield(scripting_api.state, -1, module->name);

        for (scripting_function_t *func = module->functions; func < module->functions + module->function_count; ++func) {
            log_info("Registering function '%s'", func->name);
            lua_pushcfunction(scripting_api.state, func->function);
            lua_setfield(scripting_api.state, -2, func->name);
        }

        lua_pop(scripting_api.state, 1);
    }
}

void scripting_api_cleanup(void) {
    lua_close(scripting_api.state);
    object_manager_delete(&scripting_api.manager);
}

void scripting_api_update(void) {
    scripting_api_update_globals();
    object_t *start = object_manager_get_all(&scripting_api.manager);

    lua_getglobal(scripting_api.state, "syrup");
    lua_getfield(scripting_api.state, -1, "instances");
    for (object_t *object = start; object != NULL; object = object->next) {
        lua_pushinteger(scripting_api.state, object->id);
        lua_gettable(scripting_api.state, -2);

        lua_getfield(scripting_api.state, -1, "__index");
        if (!lua_istable(scripting_api.state, -1))
            return;

        if (!lua_istable(scripting_api.state, -1)) {
            lua_pop(scripting_api.state, 1);
            // TODO: Delete it
            continue;
        }

        lua_getfield(scripting_api.state, -1, "update");
        if (lua_isfunction(scripting_api.state, -1)) {
            lua_pushvalue(scripting_api.state, -2);
            if (lua_pcall(scripting_api.state, 1, 0, 0) != LUA_OK) {
                log_error(lua_tostring(scripting_api.state, -1));
                lua_pop(scripting_api.state, 1);
            }
        } else lua_pop(scripting_api.state, 1);
        lua_pop(scripting_api.state, 1);

        lua_getfield(scripting_api.state, -1, "depth");
        if (lua_isnumber(scripting_api.state, -1)) {
            object_t *o = object_manager_get(&scripting_api.manager, object->id);
            if (o) o->depth = lua_tonumber(scripting_api.state, -1);
        }
        lua_pop(scripting_api.state, 2);
    }
    lua_pop(scripting_api.state, 2);

    object_list_t list = (object_list_t) { .start = start, };
    object_list_delete(&list);
}

void scripting_api_update_globals(void) {
    lua_getglobal(scripting_api.state, "syrup");
    lua_pushnumber(scripting_api.state, renderer.delta_time);
    lua_setfield(scripting_api.state, -2, "delta_time");

    lua_getfield(scripting_api.state, -1, "mouse");
    if (lua_isnil(scripting_api.state, -1)) {
        lua_pop(scripting_api.state, 1);
        lua_newtable(scripting_api.state);
        lua_setfield(scripting_api.state, -2, "mouse");
        lua_getfield(scripting_api.state, -1, "mouse");
    }
    double x, y;
    renderer_fbo_mouse_position(&x, &y);
    lua_pushnumber(scripting_api.state, x);
    lua_setfield(scripting_api.state, -2, "x");
    lua_pushnumber(scripting_api.state, y);
    lua_setfield(scripting_api.state, -2, "y");
    lua_pop(scripting_api.state, 2);
}

void scripting_api_draw(void) {
    object_t *start = object_manager_get_all(&scripting_api.manager);

    lua_getglobal(scripting_api.state, "syrup");
    lua_getfield(scripting_api.state, -1, "instances");
    for (object_t *object = start; object != NULL; object = object->next) {
        lua_pushinteger(scripting_api.state, object->id);
        lua_gettable(scripting_api.state, -2);

        if (!lua_istable(scripting_api.state, -1)) {
            lua_pop(scripting_api.state, 1);
            // TODO: Delete it
            continue;
        }

        lua_getfield(scripting_api.state, -1, "draw");
        if (lua_isfunction(scripting_api.state, -1)) {
            lua_pushvalue(scripting_api.state, -2);
            if (lua_pcall(scripting_api.state, 1, 0, 0) != LUA_OK) {
                log_error(lua_tostring(scripting_api.state, -1));
                lua_pop(scripting_api.state, 1);
            }
        } else lua_pop(scripting_api.state, 1);
        lua_pop(scripting_api.state, 1);
    }

    for (object_t *object = start; object != NULL; object = object->next) {
        lua_pushinteger(scripting_api.state, object->id);
        lua_gettable(scripting_api.state, -2);

        if (!lua_istable(scripting_api.state, -1)) {
            lua_pop(scripting_api.state, 1);
            // TODO: Delete it
            continue;
        }

        lua_getfield(scripting_api.state, -1, "draw_gui");
        if (lua_isfunction(scripting_api.state, -1)) {
            lua_pushvalue(scripting_api.state, -2);
            if (lua_pcall(scripting_api.state, 1, 0, 0) != LUA_OK) {
                log_error(lua_tostring(scripting_api.state, -1));
                lua_pop(scripting_api.state, 1);
            }
        } else lua_pop(scripting_api.state, 1);
        lua_pop(scripting_api.state, 1);
    }
    lua_pop(scripting_api.state, 2);

    object_list_t list = (object_list_t) { .start = start, };
    object_list_delete(&list);
}

result_t scripting_api_create(const char *type, float x, float y) {
    lua_getglobal(scripting_api.state, "syrup");
    lua_getfield(scripting_api.state, -1, "objects");
    lua_getfield(scripting_api.state, -1, type);
    if (!lua_istable(scripting_api.state, -1)) {
        lua_pop(scripting_api.state, 1);

        char *path = format("resources/objects/%s.lua", type);
        if (!fs_exists(path))
            path = format("resources/engine/objects/%s.lua", type);
        if (!fs_exists(path))
            return result_error("ObjectNotFoundError", "The object '%s' could not be loaded because it could not be found.", type);

        if (luaL_dofile(scripting_api.state, path) != 0) {
            result_t res = result_error("ObjectCodeError", lua_tostring(scripting_api.state, -1));
            lua_pop(scripting_api.state, 2);
            free(path);
            return res;
        }
        if (!lua_istable(scripting_api.state, -1)) {
            result_t res = result_error("InvalidObjectError", "Object '%s' did not return a table in its code file.", type);
            lua_pop(scripting_api.state, 2);
            free(path);
            return res;
        }

        lua_pushvalue(scripting_api.state, -1);
        lua_setfield(scripting_api.state, -2, "__index");
        lua_pushvalue(scripting_api.state, -1);
        lua_setfield(scripting_api.state, -3, type);
    }

    // Instantiate
    lua_newtable(scripting_api.state); // Object
    lua_pushvalue(scripting_api.state, -2);
    lua_setmetatable(scripting_api.state, -2);
    lua_remove(scripting_api.state, -2);
    lua_remove(scripting_api.state, -3);
    lua_remove(scripting_api.state, -4);

    if (scripting_api.manager.list_count >= 4294967295)
        panic(result_error("ObjectOverflowError", "Object count exceeded maximum unsigned integer limit. How did you even manage to do that, dude?"))\
    while (object_manager_get(&scripting_api.manager, scripting_api.current_id))
        scripting_api.current_id++;

    lua_getfield(scripting_api.state, -1, "depth");
    if (!lua_isnumber(scripting_api.state, -1)) {
        lua_pop(scripting_api.state, 1);
        lua_pushnumber(scripting_api.state, 0);
        lua_setfield(scripting_api.state, -2, "depth");
        lua_getfield(scripting_api.state, -1, "depth");
    }

    // Create Object
    object_t *object = object_manager_push(&scripting_api.manager, type, scripting_api.current_id++, lua_tonumber(scripting_api.state, -1));
    lua_pop(scripting_api.state, 1);

    lua_pushnumber(scripting_api.state, object->id);
    lua_setfield(scripting_api.state, -2, "id");
    lua_pushstring(scripting_api.state, type);
    lua_setfield(scripting_api.state, -2, "type");
    lua_pushnumber(scripting_api.state, x);
    lua_setfield(scripting_api.state, -2, "x");
    lua_pushnumber(scripting_api.state, y);
    lua_setfield(scripting_api.state, -2, "y");

    // Insert into instances
    lua_getglobal(scripting_api.state, "syrup");
    lua_getfield(scripting_api.state, -1, "instances");
    lua_pushinteger(scripting_api.state, object->id);
    lua_pushvalue(scripting_api.state, -4);
    lua_settable(scripting_api.state, -3);
    lua_pop(scripting_api.state, 2);

    // Call start
    lua_getfield(scripting_api.state, -1, "start");
    if (!lua_isfunction(scripting_api.state, -1)) {
        lua_pop(scripting_api.state, 1);
        return result_no_error();
    }
    lua_pushvalue(scripting_api.state, -2);
    if (lua_pcall(scripting_api.state, 1, 0, 0) != 0) {
        log_error(lua_tostring(scripting_api.state, -1));
        lua_pop(scripting_api.state, 1);
    }
    return result_no_error();
}
