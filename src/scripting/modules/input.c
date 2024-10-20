#include "input.h"
#include "modules.h"
#include "../../input/keyboard_mouse.h"
#include "../../graphics/renderer.h"
#include "../../resources/resource_manager.h"
#include "../../util/log.h"
#include <GLFW/glfw3.h>
#include <lauxlib.h>
#include <lua.h>
#include <string.h>

scripting_function_t api_input_functions[] = {
    { "is_key_pressed", api_input_is_key_pressed },
    { "is_key_down", api_input_is_key_down },
    { "is_mouse_button_pressed",  api_input_is_mouse_button_pressed},
    { "is_mouse_button_down", api_input_is_mouse_button_down },
    { "keyboard_string", api_input_keyboard_string },
    { "set_cursor", api_input_set_cursor },
};

__attribute__((constructor)) void api_input_init(void) {
    scripting_modules[SCRIPTING_MODULE_INPUT] = (scripting_module_t) {
        .name = "input",
        .function_count = sizeof(api_input_functions) / sizeof(scripting_function_t),
        .functions = api_input_functions,
    };
}

int api_input_is_key_pressed(lua_State *L) {
    lua_pushboolean(L, is_key_pressed(luaL_checkinteger(L, 1)));
    return 1;
}

int api_input_is_key_down(lua_State *L) {
    lua_pushboolean(L, is_key_down(luaL_checkinteger(L, 1)));
    return 1;
}

int api_input_is_mouse_button_pressed(lua_State *L) {
    lua_pushboolean(L, is_mouse_button_pressed(luaL_checkinteger(L, 1)));
    return 1;
}

int api_input_is_mouse_button_down(lua_State *L) {
    lua_pushboolean(L, is_mouse_button_down(luaL_checkinteger(L, 1)));
    return 1;
}

int api_input_keyboard_string(lua_State *L) {
    char *c = keyboard_mouse.keyboard_string + KEYBOARD_STRING_COUNT - 2;
    while (c != keyboard_mouse.keyboard_string && *(c-1) != '\0') c--;
    lua_pushstring(L, c);
    memset(&keyboard_mouse.keyboard_string, 0, KEYBOARD_STRING_COUNT);
    return 1;
}

int api_input_set_cursor(lua_State *L) {
    int curs = luaL_checkinteger(L, 1);
    GLFWcursor *cursor = resource_manager_cursor(curs);
    if (!cursor) {
        log_error("Unable to locate cursor '%d'.", curs);
        return 0;
    }
    if (cursor != keyboard_mouse.cursor) {
        glfwSetCursor(renderer.window_handle, cursor);
        keyboard_mouse.cursor = cursor;
    }
    return 0;
}