#include "input.h"
#include "modules.h"
#include "../../input/keyboard_mouse.h"
#include <lauxlib.h>

scripting_function_t api_input_functions[] = {
    { "is_key_pressed", api_input_is_key_pressed },
    { "is_key_down", api_input_is_key_down }
};

__attribute__((constructor)) void api_input_init(void) {
    scripting_modules[SCRIPTING_MODULE_INPUT] = (scripting_module_t) {
        .name = "input",
        .function_count = sizeof(api_input_functions) / sizeof(scripting_function_t),
        .functions = api_input_functions,
    };
}

int api_input_is_key_pressed(lua_State *L) {
    return is_key_pressed(luaL_checkinteger(L, 1));
}

int api_input_is_key_down(lua_State *L) {
    return is_key_down(luaL_checkinteger(L, 1));
}