#include "modules.h"
#include "objects.h"
#include "../../util/log.h"
#include "../../util/stringext.h"
#include "../../structures/result.h"
#include "../../scripting/scripting_api.h"
#include <lauxlib.h>
#include <lua.h>
#include <stdint.h>
#include <wchar.h>

scripting_function_t api_objects_functions[] = {
    { "new", api_objects_new },
    { "delete", api_objects_delete },
    { "get",  api_objects_get },
    { "get_all", api_objects_get_all },
};

__attribute__((constructor)) void api_objects_init(void) {
    scripting_modules[SCRIPTING_MODULE_OBJECTS] = (scripting_module_t) {
        .name = "objects",
        .function_count = sizeof(api_objects_functions) / sizeof(scripting_function_t),
        .functions = api_objects_functions,
    };
}

int api_objects_new(lua_State *L) {
    result_t res = scripting_api_push(luaL_checkstring(L, 1), luaL_checknumber(L, 2), luaL_checknumber(L, 3), luaL_checknumber(L, 4), lua_toboolean(L, 5), luaL_checkstring(L, 6));
    if (res.is_error) {
        log_error("Failed to load object '%s': %s", res.description);
        result_discard(res);
        return 0;
    }
    return 1;
}

int api_objects_delete(lua_State *L) {
    scripting_api_delete(luaL_checknumber(L, 1));
    return 0;
}

int api_objects_get(lua_State *L) {
    lua_getglobal(scripting_api.state, "syrup");
    lua_getfield(scripting_api.state, -1, "instances");
    lua_pushinteger(scripting_api.state, luaL_checkinteger(L, 1));
    lua_gettable(L, -2);
    lua_remove(L, -3);
    lua_remove(L, -2);
    return 1;
}

int api_objects_get_all(lua_State *L) {
    const char *type = luaL_checkstring(L, 1);
    object_t *objects = object_manager_get_all(&scripting_api.manager);
    lua_newtable(L);
    if (!objects) {
        return 1;
    }

    lua_getglobal(scripting_api.state, "syrup");
    lua_getfield(scripting_api.state, -1, "instances");
    uint32_t i = 0;
    for (object_t *object = objects; object; object = object->next) {
        lua_pushinteger(L, i);
        lua_pushinteger(scripting_api.state, object->id);
        lua_gettable(L, -3);
        if (!lua_istable(L, -1)) {
            lua_pop(L, 1);
            continue;
        }

        lua_getfield(L, -1, "type");
        bool is_type = bstrcmp(type, lua_tostring(L, -1));
        lua_pop(L, 1);

        uint32_t to_pop = 0;
        while (!is_type) {
            if (!lua_getmetatable(L, -1)) {
                break;
            }
            to_pop++;

            lua_getfield(L, -1, "__index");
            if (!lua_istable(L, -1)) {
                lua_pop(L, 1);
                break;
            }
            to_pop++;

            lua_getfield(L, -1, "type");
            if (!lua_isstring(L, -1)) {
                lua_pop(L, 1);
                break;
            }
            to_pop++;

            if (bstrcmp(type, lua_tostring(L, -1))) {
                is_type = true;
                break;
            }
        }
        lua_pop(L, to_pop);

        if (is_type) {
            lua_settable(L, -5);
            i++;
        } else lua_pop(L, 2);
    }
    lua_pop(L, 2);

    object_list_t list = { .type = NULL, .start = objects };
    object_list_delete(&list);
    return 1;
}