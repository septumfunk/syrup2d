//? septumfunk 2024
#include "window.h"
#include "../game/game.h"
#include "../win32/msgbox.h"
#include "../util/ext.h"
#include "../data/result.h"
#include <GLFW/glfw3.h>
#include <cglm/cam.h>
#include <stdlib.h>
#include <glad/glad.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

window_t window;

void window_init(char *title, uint16_t width, uint16_t height) {
    memset(&window, 0, sizeof(window_t));
    // Info
    window.title = calloc(1, strlen(title) + 1);
    strcpy(window.title, title);

    window.dimensions[0] = width;
    window.dimensions[1] = height;

    // Init
    if (!glfwInit())
        panic(error("GLFWInitializeError", "Failed to initialize GLFW."));

    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    window._clear_color = (gl_color_t){ 0.2f, 0.3f, 0.3f, 1.0f };

    if ((window._handle = glfwCreateWindow(width, height, title, NULL, NULL)) == NULL)
        panic(error("GLFWWindowError", "Failed to open GLFW window."));

    // Callbacks
    glfwSetErrorCallback(_window_error_cb);

    // Load GLAD
    glfwMakeContextCurrent(window._handle);
    if (!gladLoadGL())
        panic(error("GLADInitializeError", "Failed to initialize GLAD."));

    // Depth and Blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Ready
    glViewport(0, 0, width, height);
    glfwSwapInterval(1);
    glfwShowWindow(window._handle);
}

void window_cleanup(void) {
    glfwSetWindowShouldClose(window._handle, true);
    if (window._handle)
        glfwDestroyWindow(window._handle);
    free(window.title);
}

bool window_loop(void) {
    window._last_time = glfwGetTime();

    glClearColor(
        window._clear_color.r,
        window._clear_color.g,
        window._clear_color.b,
        window._clear_color.a
    );

    for (GLenum err = glGetError(); err != GL_NO_ERROR; err = glGetError())
        msgbox_warn("OpenGL Error", "OpenGL encountered an error: %d", err);

    return !glfwWindowShouldClose(window._handle);
}

void window_swap(void) {
    glfwSwapBuffers(window._handle);
    glfwPollEvents();

    window.delta_time = glfwGetTime() - window._last_time;
    window.fps = 1.0f / window.delta_time;
}

void window_set_title(char *title) {
    strcpy(window.title, title);
    glfwSetWindowTitle(window._handle, title);
}

void window_resize(uint16_t width, uint16_t height) {
    glfwSetWindowSize(window._handle, width, height);
}

void window_set_clear_color(color_t color) {
    window._clear_color = color_to_gl(color);
}

void _window_resize_cb(unused GLFWwindow *handle, int width, int height) {
    glViewport(0, 0, width, height);
    window.dimensions[0] = width;
    window.dimensions[1] = height;
}

void _window_error_cb(int error_code, const char* description) {
    msgbox_error("OpenGL Error", "OpenGL Error:\n%s\n\nError Code: %d", description, error_code);
}
