#include "resources.h"
#include "../../resources/resource_manager.h"
#include <lauxlib.h>
#include <lua.h>

scripting_function_t api_resource_functions[] = {
    { "import_sprite", api_resources_import_sprite },
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