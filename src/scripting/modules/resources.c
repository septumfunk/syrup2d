#include "resources.h"
#include "../../engine.h"
#include "../../resources/resource_manager.h"
#include <lauxlib.h>
#include <lua.h>
#include <stdlib.h>
#include <string.h>

scripting_function_t api_resource_functions[] = {
    { "import_sprite", api_resources_import_sprite },
    { "get_game_data", api_resources_get_game_data },
    { "set_game_data", api_resources_set_game_data },
};

__attribute__((constructor)) void api_resources_init(void) {
    scripting_modules[SCRIPTING_MODULE_RESOURCES] = (scripting_module_t) {
        .name = "resources",
        .function_count = sizeof(api_resource_functions) / sizeof(scripting_function_t),
        .functions = api_resource_functions,
    };
}

int api_resources_import_sprite(lua_State *L) {
    result_t res = resource_manager_import_sprite(luaL_checkstring(L, 1), luaL_checkinteger(L, 2), luaL_checkinteger(L, 3));
    if (res.is_error) {
        luaL_error(L, res.description);
        result_discard(res);
    }
    return 0;
}

int api_resources_get_game_data(lua_State *L) {
    lua_newtable(L);

    lua_pushstring(L, resource_manager.game_data.title);
    lua_setfield(L, -2, "title");

    lua_pushinteger(L, resource_manager.game_data.width);
    lua_setfield(L, -2, "width");
    lua_pushinteger(L, resource_manager.game_data.height);
    lua_setfield(L, -2, "height");
    lua_pushinteger(L, resource_manager.game_data.window_scale);
    lua_setfield(L, -2, "window_scale");

    return 1;
}

int api_resources_set_game_data(lua_State *L) {
    const char *title = luaL_checkstring(L, 1);
    int width = max(luaL_checkinteger(L, 2), 256);
    int height = max(luaL_checkinteger(L, 3), 144);
    int window_scale = max(luaL_checkinteger(L, 4), 1);

    free(resource_manager.game_data.title);
    resource_manager.game_data.title = _strdup(title);
    resource_manager.game_data.width = width;
    resource_manager.game_data.height = height;
    resource_manager.game_data.window_scale = window_scale;
    resource_manager_save_game_data("resources");
    engine_stop();

    return 0;
}