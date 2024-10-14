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
    glfwSetMouseButtonCallback(window._handle, _controller_mouse_button_cb);
    glfwSetCharCallback(window._handle, _controller_char_cb);

    // Lua
    lua_pushcfunction(object_controller.state, lua_is_key_down);
    lua_setglobal(object_controller.state, "is_key_down");
    lua_pushcfunction(object_controller.state, lua_is_key_pressed);
    lua_setglobal(object_controller.state, "is_key_pressed");
    lua_pushcfunction(object_controller.state, lua_keyboard_string);
    lua_setglobal(object_controller.state, "keyboard_string");

    lua_pushcfunction(object_controller.state, lua_is_mouse_button_pressed);
    lua_setglobal(object_controller.state, "is_mouse_button_pressed");
    lua_pushcfunction(object_controller.state, lua_is_mouse_button_down);
    lua_setglobal(object_controller.state, "is_mouse_button_down");
}

void controller_reset(void) {
    memset(controller.keys_pressed, 0, KEY_COUNT);
    memset(controller.mouse_buttons_pressed, 0, MOUSE_BUTTON_COUNT);
}

bool is_key_pressed(key_e key) {
    return controller.keys_pressed[key];
}

int lua_is_key_pressed(lua_State *L) {
    lua_pushboolean(L, is_key_pressed(luaL_checkinteger(L, 1)));
    return 1;
}

bool is_key_down(key_e key) {
    return glfwGetKey(window._handle, key) == GLFW_PRESS;
}

int lua_is_key_down(lua_State *L) {
    lua_pushboolean(L, is_key_down(luaL_checkinteger(L, 1)));
    return 1;
}

bool is_mouse_button_pressed(mouse_button_e mouse_button) {
    return controller.mouse_buttons_pressed[mouse_button];
}

int lua_is_mouse_button_pressed(lua_State *L) {
    lua_pushboolean(L, is_mouse_button_pressed(luaL_checkinteger(L, 1)));
    return 1;
}

bool is_mouse_button_down(mouse_button_e mouse_button) {
    return glfwGetMouseButton(window._handle, mouse_button);
}

int lua_is_mouse_button_down(lua_State *L) {
    lua_pushboolean(L, is_mouse_button_down(luaL_checknumber(L, 1)));
    return 1;
}

bool check_code(const char *code) {
    bool check = bstrcmp(code, controller.keyboard_string + KEYBOARD_STRING_COUNT - 1 - strlen(code));
    if (check)
        memset(controller.keyboard_string, 0, KEYBOARD_STRING_COUNT);
    return check;
}

int lua_keyboard_string(lua_State *L) {
    char *s = controller.keyboard_string + KEYBOARD_STRING_COUNT - 1;
    while (*(s - 1) != '\0' && s != controller.keyboard_string)
        s--;
    lua_pushstring(L, s);
    memset(controller.keyboard_string, 0, KEYBOARD_STRING_COUNT);
    return 1;
}

void _controller_keyboard_cb(unused GLFWwindow* handle, int key, unused int scancode, int action, unused int mods) {
    if (action == GLFW_PRESS)
        controller.keys_pressed[key] = true;
}

void _controller_mouse_button_cb(unused GLFWwindow* window, int button, int action, unused int mods) {
    if (action == GLFW_PRESS)
        controller.mouse_buttons_pressed[button] = true;
}

void _controller_char_cb(unused GLFWwindow* window, unsigned int codepoint) {
    memcpy(controller.keyboard_string, controller.keyboard_string + 1, KEYBOARD_STRING_COUNT - 2);
    controller.keyboard_string[KEYBOARD_STRING_COUNT - 2] = (unsigned char)codepoint;
}
