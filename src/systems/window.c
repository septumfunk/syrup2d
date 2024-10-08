//? septumfunk 2024
#include "window.h"
#include "../game/game.h"
#include "../win32/msgbox.h"
#include "../util/ext.h"
#include <GLFW/glfw3.h>
#include <cglm/cam.h>
#include <glad/glad.h>
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
    window->dimensions[1] = height;

    // Init
    if (!glfwInit()) {
        msgbox_error("OpenGL Error", "Failed to Initialize GLFW");
        game_end();
    }

    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    if ((window->_handle = glfwCreateWindow(width, height, title, NULL, NULL)) == NULL) {
        msgbox_error("OpenGL Error", "Failed to open GLFW window.");
        game_end();
    }

    // Callbacks
    glfwSetErrorCallback(_window_error_cb);

    // Load GLAD
    glfwMakeContextCurrent(window->_handle);
    if (!gladLoadGL()) {
        msgbox_error("OpenGL Error", "Failed to get initialize GLAD.");
        game_end();
    }

    // Texture options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Ortho
    glm_ortho(0, width, height, 0, -1, 1, (vec4 *)&window->ortho);

    // Ready
    glViewport(0, 0, width, height);
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

    for (GLenum err = glGetError(); err != GL_NO_ERROR; err = glGetError())
        msgbox_warn("OpenGL Error", "OpenGL encountered an error: %d", err);

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

void window_resize(uint16_t width, uint16_t height) {
    window->dimensions[0] = width;
    window->dimensions[1] = height;
    glViewport(0, 0, width, height);
}

void window_set_clear_color(color_t color) {
    window->_clear_color = color_to_gl(color);
}

void _window_resize_cb(unused GLFWwindow *handle, int width, int height) {
    glViewport(0, 0, width, height);
    window->dimensions[0] = width;
    window->dimensions[1] = height;
}

void _window_error_cb(int error_code, const char* description) {
    msgbox_error("OpenGL Error", "OpenGL Error:\n%s\n\nError Code: %d", description, error_code);
}
