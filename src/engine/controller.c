//? septumfunk 2024

#include "controller.h"
#include "window.h"
#include "object_controller.h"
#include "../data/stringext.h"
#include <GLFW/glfw3.h>
#include <lauxlib.h>
#include <lua.h>
#include <string.h>

controller_t controller;

void controller_init(void) {
    memset(&controller, 0, sizeof(controller_t));
    controller_reset();
    memset(controller.keyboard_string, 0, KEYBOARD_STRING_COUNT);
    glfwSetKeyCallback(window._handle, _controller_keyboard_cb);
    glfwSetCharCallback(window._handle, _controller_char_cb);

    // Lua
    lua_pushcfunction(object_controller.state, lua_is_key_down);
    lua_setglobal(object_controller.state, "is_key_down");
    lua_pushcfunction(object_controller.state, lua_is_key_pressed);
    lua_setglobal(object_controller.state, "is_key_pressed");
}

void controller_reset(void) {
    memset(controller.keys_pressed, 0, KEY_COUNT);
}

bool is_key_pressed(key_e key) {
    return controller.keys_pressed[key];
}

int lua_is_key_pressed(lua_State *L) {
    int key = (int)luaL_checknumber(L, 1);
    lua_pushboolean(L, is_key_pressed(key));
    return 1;
}

bool is_key_down(key_e key) {
    return glfwGetKey(window._handle, key) == GLFW_PRESS;
}

int lua_is_key_down(lua_State *L) {
    int key = (int)luaL_checknumber(L, 1);
    lua_pushboolean(L, is_key_down(key));
    return 1;
}

bool check_code(const char *code) {
    bool check = bstrcmp(code, controller.keyboard_string + KEYBOARD_STRING_COUNT - 1 - strlen(code));
    if (check)
        memset(controller.keyboard_string, 0, KEYBOARD_STRING_COUNT);
    return check;
}

void _controller_keyboard_cb(unused GLFWwindow* handle, int key, unused int scancode, int action, unused int mods) {
    if (action == GLFW_PRESS)
        controller.keys_pressed[key] = true;
    if (action == GLFW_RELEASE)
        controller.keys_pressed[key] = false;
}

void _controller_char_cb(unused GLFWwindow* window, unsigned int codepoint) {
    memcpy(controller.keyboard_string, controller.keyboard_string + 1, KEYBOARD_STRING_COUNT - 2);
    controller.keyboard_string[KEYBOARD_STRING_COUNT - 2] = (unsigned char)codepoint;
}
