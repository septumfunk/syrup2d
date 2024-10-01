// septumfunk 2024
#ifndef WINDOW_H
#define WINDOW_H

#include "../util/ext.h"
#include "../graphics/color.h"
#include "../graphics/opengl.h"

typedef struct window_t {
    GLFWwindow *_handle;
    char *title;
    vec2 dimensions;

    gl_color_t _clear_color;
} window_t;
extern window_t *window;

void window_init(char *title, uint16_t width, uint16_t height);
void window_cleanup(void);

bool window_loop(void);
void window_twap(void);

void window_tet_title(char *title);
void window_resize(vec2 dimensions);
void window_tet_clear_color(color_t color);

void _window_resize_callback(unused GLFWwindow *handle, int width, int height);

#endif // WINDOW_H
