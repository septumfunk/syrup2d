#include "window.h"
#include "../game/game.h"
#include "../win32/msgbox.h"
#include <GLFW/glfw3.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

window_t *window;

void window_init(char *title, uint16_t width, uint16_t height) {
    // Allocate
    window = calloc(1, sizeof(window_t));

    // Info
    window->title = calloc(1, strlen(title) + 1);
    strcpy(window->title, title);

    window->dimensions[0] = width;
    window->dimensions[1] = width;

    // Init
    if (!glfwInit()) {
        msgbox_error("OpenGL Error", "Failed to Initialize GLFW");
        game_end();
    }

    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE); // Not visible until init is completed.
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    if ((window->_handle = glfwCreateWindow(width, height, title, NULL, NULL)) == NULL) {
        msgbox_error("OpenGL Error", "Failed to open GLFW window.");
        game_end();
    }

    // Callbacks
    glfwSetErrorCallback(display_glfw_error);

    glfwMakeContextCurrent(window->_handle);
    if (!gladLoadGL()) {
        msgbox_error("OpenGL Error", "Failed to get initialize GLAD.");
        game_end();
    }

    glfwSwapInterval(0); // Disable V-Sync
    glfwShowWindow(window->_handle);
}

void window_cleanup(void) {
    glfwSetWindowShouldClose(window->_handle, true);
    if (window->_handle)
        glfwDestroyWindow(window->_handle);
    free(window->title);
    free(window);
}

bool window_loop(void) {
    glClearColor(
        window->_clear_color.r,
        window->_clear_color.g,
        window->_clear_color.b,
        window->_clear_color.a
    );
    glClear(GL_COLOR_BUFFER_BIT);
    return !glfwWindowShouldClose(window->_handle);
}

void window_swap(void) {
    glfwSwapBuffers(window->_handle);
    glfwPollEvents();
}

void window_set_title(char *title) {
    strcpy(window->title, title);
    glfwSetWindowTitle(window->_handle, title);
}

void window_resize(vec2 dimensions) {
    memcpy(window->dimensions, dimensions, sizeof(vec2));
    glViewport(0, 0, dimensions[0], dimensions[1]);
}

void window_set_clear_color(color_t color) {
    window->_clear_color = color_to_gl(color);
}

void _window_resize_callback(unused GLFWwindow *handle, int width, int height) {
    glViewport(0, 0, width, height);
    window->dimensions[0] = width;
    window->dimensions[1] = height;
}
