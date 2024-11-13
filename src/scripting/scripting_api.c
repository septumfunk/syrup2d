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
#include <stdio.h>
#include <stdlib.h>
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

    char *path = format("package.path = package.path .. ';%s/scripts/?.lua;%s/.syrup/scripts/?.lua;%s/scripts/?/init.lua;%s/.syrup/scripts/?/init.lua'", resource_manager.folder, resource_manager.folder, resource_manager.folder, resource_manager.folder);
    luaL_dostring(
        scripting_api.state,
        path
    );
    free(path);
}

void scripting_api_init_globals(void) {
    lua_newtable(scripting_api.state);
    lua_setglobal(scripting_api.state, "syrup");
    lua_getglobal(scripting_api.state, "syrup");

    lua_newtable(scripting_api.state);
    lua_setfield(scripting_api.state, -2, "cache");
    lua_newtable(scripting_api.state);
    lua_setfield(scripting_api.state, -2, "instances");

    lua_pop(scripting_api.state, 2);

    scripting_api_update_globals();
}

void scripting_api_init_modules(void) {
    log_header("Initializing Scripting Modules");
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

        if (!lua_istable(scripting_api.state, -1)) {
            lua_pop(scripting_api.state, 1);
            // TODO: Delete it
            continue;
        }

        lua_getfield(scripting_api.state, -1, "enabled");
        if (!lua_isboolean(scripting_api.state, -1) || !lua_toboolean(scripting_api.state, -1)) {
            lua_pop(scripting_api.state, 2);
            continue;
        }
        lua_pop(scripting_api.state, 1);

        lua_getfield(scripting_api.state, -1, "update");
        if (lua_isfunction(scripting_api.state, -1)) {
            lua_pushvalue(scripting_api.state, -2);
            if (lua_pcall(scripting_api.state, 1, 0, 0) != LUA_OK) {
                const char *err = lua_tostring(scripting_api.state, -1);
                luaL_traceback(scripting_api.state, scripting_api.state, err, 2);
                log_error(lua_tostring(scripting_api.state, -1));
                lua_pop(scripting_api.state, 2);
            }
        } else lua_pop(scripting_api.state, 1);

        lua_getfield(scripting_api.state, -1, "z");
        if (lua_isnumber(scripting_api.state, -1)) {
            object_t *o = object_manager_get(&scripting_api.manager, object->id);
            if (o) o->z = lua_tonumber(scripting_api.state, -1);
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
    lua_pop(scripting_api.state, 1);

    lua_newtable(scripting_api.state);
    lua_pushnumber(scripting_api.state, renderer.corrected_dimensions.width);
    lua_setfield(scripting_api.state, -2, "width");
    lua_pushnumber(scripting_api.state, renderer.corrected_dimensions.height);
    lua_setfield(scripting_api.state, -2, "height");
    lua_setfield(scripting_api.state, -2, "dimensions");

    lua_pushstring(scripting_api.state, resource_manager.current_scene ? resource_manager.current_scene->name : "Unknown");

    lua_pop(scripting_api.state, 1);
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

        lua_getfield(scripting_api.state, -1, "enabled");
        if (!lua_isboolean(scripting_api.state, -1) || !lua_toboolean(scripting_api.state, -1)) {
            lua_pop(scripting_api.state, 2);
            continue;
        }
        lua_pop(scripting_api.state, 1);

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

    renderer.gui = true;
    renderer_update_buffer();
    for (object_t *object = start; object != NULL; object = object->next) {
        lua_pushinteger(scripting_api.state, object->id);
        lua_gettable(scripting_api.state, -2);

        if (!lua_istable(scripting_api.state, -1)) {
            lua_pop(scripting_api.state, 1);
            // TODO: Delete it
            continue;
        }

        lua_getfield(scripting_api.state, -1, "draw_ui");
        if (lua_isfunction(scripting_api.state, -1)) {
            lua_pushvalue(scripting_api.state, -2);
            if (lua_pcall(scripting_api.state, 1, 0, 0) != LUA_OK) {
                log_error(lua_tostring(scripting_api.state, -1));
                lua_pop(scripting_api.state, 1);
            }
        } else lua_pop(scripting_api.state, 1);
        lua_pop(scripting_api.state, 1);
    }
    renderer.gui = false;
    renderer_update_buffer();
    lua_pop(scripting_api.state, 2);

    object_list_t list = (object_list_t) { .start = start, };
    object_list_delete(&list);
}

result_t scripting_api_push(const char *type, float x, float y, float z, float enabled, const char *creation_code) {
    hashtable_t inherited = hashtable_string();
    bool t = true;
    char *to_load = _strdup(type);
    hashtable_insert(&inherited, (void *)type, &t, sizeof(bool));
    uint32_t to_pop = 0;
    while (true) {
        char *path = format(OBJECT_PATH, resource_manager.folder, to_load);
        if (!fs_exists(path)) {
            free(path);
            path = format(OBJECT_ENGINE_PATH, resource_manager.folder, to_load);
        }
        if (luaL_dofile(scripting_api.state, path) != LUA_OK) {
            result_t res = result_error("LuaFileError", lua_tostring(scripting_api.state, -1));
            lua_pop(scripting_api.state, 1);
            return res;
        }
        free(path);

        if (!lua_istable(scripting_api.state, -1))
            return result_error("LuaTableError", "Object '%s' failed to return a table.", to_load);

        if (to_pop) {
            lua_newtable(scripting_api.state);
            lua_pushvalue(scripting_api.state, -2);
            lua_pushstring(scripting_api.state, to_load);
            lua_setfield(scripting_api.state, -2, "type");
            lua_setfield(scripting_api.state, -2, "__index");
            lua_setmetatable(scripting_api.state, -3);
        }
        to_pop++;

        // Single Inheritance
        lua_getfield(scripting_api.state, -1, "extends");
        if (lua_tostring(scripting_api.state, -1)) {
            const char *extends = lua_tostring(scripting_api.state, -1);

            if (hashtable_get(&inherited, (void *)extends))
                panic(result_error("CircularImportError", "Object '%s' inherits itself somewhere along the hierarchy.", extends));
            hashtable_insert(&inherited, (void *)type, &t, sizeof(bool));

            free(to_load);
            to_load = _strdup(extends);
            lua_pop(scripting_api.state, 1); // "extends"
            continue;
        }
        lua_pop(scripting_api.state, to_pop);
        hashtable_delete(&inherited);
        free(to_load);
        break;
    }

    while (object_manager_get(&scripting_api.manager, scripting_api.current_id))
        scripting_api.current_id++;

    lua_getfield(scripting_api.state, -1, "z");
    if (!lua_isnumber(scripting_api.state, -1)) {
        lua_pop(scripting_api.state, 1);
        lua_pushnumber(scripting_api.state, z);
        lua_setfield(scripting_api.state, -2, "z");
        lua_getfield(scripting_api.state, -1, "z");
    }

    // Create Object
    object_manager_push(&scripting_api.manager, type, scripting_api.current_id, lua_tonumber(scripting_api.state, -1));
    object_t *object = object_manager_get(&scripting_api.manager, scripting_api.current_id++);
    lua_pop(scripting_api.state, 1);

    lua_pushboolean(scripting_api.state, enabled);
    lua_setfield(scripting_api.state, -2, "enabled");
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

    // Functions
    lua_pushcfunction(scripting_api.state, api_internal_base_start);
    lua_setfield(scripting_api.state, -2, "base_start");
    lua_pushcfunction(scripting_api.state, api_internal_base_update);
    lua_setfield(scripting_api.state, -2, "base_update");
    lua_pushcfunction(scripting_api.state, api_internal_base_draw);
    lua_setfield(scripting_api.state, -2, "base_draw");
    lua_pushcfunction(scripting_api.state, api_internal_base_draw_ui);
    lua_setfield(scripting_api.state, -2, "base_draw_ui");
    lua_pushcfunction(scripting_api.state, api_internal_base_clean_up);
    lua_setfield(scripting_api.state, -2, "base_clean_up");

    // Call start
    if (enabled) {
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

        if (luaL_dostring(scripting_api.state, creation_code) != LUA_OK) {
            log_error(lua_tostring(scripting_api.state, -1));
            lua_pop(scripting_api.state, 1);
        }
    }

    return result_no_error();
}

result_t scripting_api_create(const char *type, float x, float y, float z, float enabled, const char *creation_code) {
    result_t res = scripting_api_push(type, x, y, z, enabled, creation_code);
    if (res.is_error)
        return res;
    lua_pop(scripting_api.state, 1);
    return result_no_error();
}

void scripting_api_delete(uint32_t id) {
    lua_getglobal(scripting_api.state, "syrup");
    lua_getfield(scripting_api.state, -1, "instances");
    lua_pushinteger(scripting_api.state, id);
    lua_gettable(scripting_api.state, -2);

    if (!lua_istable(scripting_api.state, -1)) {
        lua_pop(scripting_api.state, 3);
        return;
    }

    // Clean up
    lua_getfield(scripting_api.state, -1, "clean_up");
    if (lua_isfunction(scripting_api.state, -1)) {
        lua_pushvalue(scripting_api.state, -2);
        if (lua_pcall(scripting_api.state, 1, 0, 0) != LUA_OK) {
            log_error(lua_tostring(scripting_api.state, -1));
            lua_pop(scripting_api.state, 1);
        }
    } else lua_pop(scripting_api.state, 1);
    lua_pop(scripting_api.state, 1);

    lua_pushinteger(scripting_api.state, id);
    lua_pushnil(scripting_api.state);
    lua_settable(scripting_api.state, -3);
    lua_pop(scripting_api.state, 2);

    object_manager_remove(&scripting_api.manager, id);
}

void scripting_api_copy_table(void) {
    lua_newtable(scripting_api.state); // Object
    lua_pushnil(scripting_api.state);
    while(lua_next(scripting_api.state, -3) != 0) {
        lua_pushvalue(scripting_api.state, -2);
        lua_insert(scripting_api.state, -2);
        lua_settable(scripting_api.state, -4);
    }
}

void scripting_api_dump_stack(void) {
    system("cls");
    int top = lua_gettop(scripting_api.state);
    printf("INDEX  TYPE     VALUE\n");
    for (int i = 1; i <= top; i++) {
        printf("-%-5d %-8s ", top - i + 1, luaL_typename(scripting_api.state, i));
        switch (lua_type(scripting_api.state, i)) {
        case LUA_TNUMBER:
            printf("%-30g\n", lua_tonumber(scripting_api.state, i));
            break;
        case LUA_TSTRING:
            printf("'%-30s'\n",lua_tostring(scripting_api.state, i));
            break;
        case LUA_TBOOLEAN:
            printf("%-32s\n", (lua_toboolean(scripting_api.state, i) ? "true" : "false"));
            break;
        case LUA_TNIL:
            printf("%-32s\n", "nil");
            break;
        default:
            printf("%-32p\n", lua_topointer(scripting_api.state,i));
            break;
        }
    }
}

int api_internal_base_start(lua_State *L) {
    luaL_checktype(L, 1, LUA_TTABLE);
    if (!luaL_getmetafield(L, -1, "__index") || !lua_istable(L, -1)) {
        lua_pop(L, 1);
        return 0;
    }
    if (!lua_getfield(L, -1, "start") || !lua_isfunction(L, -1)) {
        lua_pop(L, 2);
        return 0;
    }
    lua_pushvalue(L, -3);
    if (lua_pcall(L, 1, 0, 0) != LUA_OK) {
        log_error(lua_tostring(L, -1));
    }
    lua_pop(L, 2);
    return 0;
}

int api_internal_base_update(lua_State *L) {
    luaL_checktype(L, 1, LUA_TTABLE);
    if (!luaL_getmetafield(L, -1, "__index") || !lua_istable(L, -1)) {
        lua_pop(L, 1);
        return 0;
    }
    if (!lua_getfield(L, -1, "update") || !lua_isfunction(L, -1)) {
        lua_pop(L, 2);
        return 0;
    }
    lua_pushvalue(L, -3);
    if (lua_pcall(L, 1, 0, 0) != LUA_OK) {
        log_error(lua_tostring(L, -1));
    }
    lua_pop(L, 2);
    return 0;
}

int api_internal_base_draw(lua_State *L) {
    luaL_checktype(L, 1, LUA_TTABLE);
    if (!luaL_getmetafield(L, -1, "__index") || !lua_istable(L, -1)) {
        lua_pop(L, 1);
        return 0;
    }
    if (!lua_getfield(L, -1, "draw") || !lua_isfunction(L, -1)) {
        lua_pop(L, 2);
        return 0;
    }
    lua_pushvalue(L, -3);
    if (lua_pcall(L, 1, 0, 0) != LUA_OK) {
        log_error(lua_tostring(L, -1));
    }
    lua_pop(L, 2);
    return 0;
}

int api_internal_base_draw_ui(lua_State *L) {
    luaL_checktype(L, 1, LUA_TTABLE);
    if (!luaL_getmetafield(L, -1, "__index") || !lua_istable(L, -1)) {
        lua_pop(L, 1);
        return 0;
    }
    if (!lua_getfield(L, -1, "draw_ui") || !lua_isfunction(L, -1)) {
        lua_pop(L, 2);
        return 0;
    }
    lua_pushvalue(L, -3);
    if (lua_pcall(L, 1, 0, 0) != LUA_OK) {
        log_error(lua_tostring(L, -1));
    }
    lua_pop(L, 2);
    return 0;
}

int api_internal_base_clean_up(lua_State *L) {
    luaL_checktype(L, 1, LUA_TTABLE);
    if (!luaL_getmetafield(L, -1, "__index") || !lua_istable(L, -1)) {
        lua_pop(L, 1);
        return 0;
    }
    if (!lua_getfield(L, -1, "clean_up") || !lua_isfunction(L, -1)) {
        lua_pop(L, 2);
        return 0;
    }
    lua_pushvalue(L, -3);
    if (lua_pcall(L, 1, 0, 0) != LUA_OK) {
        log_error(lua_tostring(L, -1));
    }
    lua_pop(L, 2);
    return 0;
}