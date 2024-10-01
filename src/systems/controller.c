// septumfunk 2024

#include "controller.h"
#include "window.h"
#include <GLFW/glfw3.h>
#include <string.h>

controller_t *controller;

void controller_init(void) {
    controller = calloc(1, sizeof(controller_t));
    controller_reset();

    glfwSetKeyCallback(window->_handle, glfw_keyboard_callback);
}

void controller_cleanup(void) {
    free(controller);
}

void controller_reset(void) {
    memset(controller->keys_pressed, 0, KEY_COUNT);
}

void glfw_keyboard_callback(unused GLFWwindow* handle, int key, unused int scancode, int action, unused int mods) {
    if (action == GLFW_PRESS)
        controller->keys_pressed[key] = true;
    if (action == GLFW_RELEASE)
        controller->keys_pressed[key] = false;
}

bool is_key_pressed(key key) {
    return controller->keys_pressed[key];
}

bool is_key_down(key key) {
    return glfwGetKey(window->_handle, key) == GLFW_PRESS;
}
