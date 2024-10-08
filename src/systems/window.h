// septumfunk 2024
#pragma once
#include "../util/ext.h"
#include "../graphics/color.h"
#include "../graphics/opengl.h"

/// OpenGL window as a system
typedef struct window_t {
    GLFWwindow *_handle;
    char *title;
    vec2 dimensions;

    gl_color_t _clear_color;
    mat4 ortho;
} window_t;
/// Window system main instance
extern window_t *window;

/// Initialize window system
void window_init(char *title, uint16_t width, uint16_t height);
/// Clean up after window system
void window_cleanup(void);

/// Prepare window for upcoming frame
/// Returns whether window should close or not
bool window_loop(void);
/// Swap buffers and clean up at the end of a frame
void window_swap(void);

/// Configure window's title
void window_set_title(char *title);
/// Configure window's dimensions
void window_resize(uint16_t width, uint16_t height);
/// Configure window's background clear color
void window_set_clear_color(color_t color);

/// GLFW window resize event callback
void _window_resize_cb(unused GLFWwindow *handle, int width, int height);
/// GLFW window error event callback
void _window_error_cb(int error_code, const char* description);