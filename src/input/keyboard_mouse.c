#include "keyboard_mouse.h"
#include "../graphics/renderer.h"
#include "../util/stringext.h"
#include <GLFW/glfw3.h>
#include <lauxlib.h>
#include <lua.h>
#include <string.h>

keyboard_mouse_t keyboard_mouse;

void keyboard_mouse_init(void) {
    memset(&keyboard_mouse, 0, sizeof(keyboard_mouse_t));
    keyboard_mouse_reset();
    memset(keyboard_mouse.keyboard_string, 0, KEYBOARD_STRING_COUNT);

    glfwSetKeyCallback(renderer.window_handle, _controller_keyboard_cb);
    glfwSetMouseButtonCallback(renderer.window_handle, _controller_mouse_button_cb);
    glfwSetCharCallback(renderer.window_handle, _controller_char_cb);
}

void keyboard_mouse_reset(void) {
    memset(keyboard_mouse.keys_pressed, 0, KEY_COUNT);
    memset(keyboard_mouse.mouse_buttons_pressed, 0, MOUSE_BUTTON_COUNT);
}

bool is_key_pressed(key_e key) {
    return keyboard_mouse.keys_pressed[key];
}

bool is_key_down(key_e key) {
    return glfwGetKey(renderer.window_handle, key) == GLFW_PRESS;
}

bool is_mouse_button_pressed(mouse_button_e mouse_button) {
    return keyboard_mouse.mouse_buttons_pressed[mouse_button];
}

bool is_mouse_button_down(mouse_button_e mouse_button) {
    return glfwGetMouseButton(renderer.window_handle, mouse_button);
}

bool check_code(const char *code) {
    bool check = bstrcmp(code, keyboard_mouse.keyboard_string + KEYBOARD_STRING_COUNT - 1 - strlen(code));
    if (check)
        memset(keyboard_mouse.keyboard_string, 0, KEYBOARD_STRING_COUNT);
    return check;
}

void _controller_keyboard_cb(unused GLFWwindow* handle, int key, unused int scancode, int action, unused int mods) {
    if (action == GLFW_PRESS)
        keyboard_mouse.keys_pressed[key] = true;
}

void _controller_mouse_button_cb(unused GLFWwindow* window, int button, int action, unused int mods) {
    if (action == GLFW_PRESS)
        keyboard_mouse.mouse_buttons_pressed[button] = true;
}

void _controller_char_cb(unused GLFWwindow* window, unsigned int codepoint) {
    memcpy(keyboard_mouse.keyboard_string, keyboard_mouse.keyboard_string + 1, KEYBOARD_STRING_COUNT - 2);
    keyboard_mouse.keyboard_string[KEYBOARD_STRING_COUNT - 2] = (unsigned char)codepoint;
}
