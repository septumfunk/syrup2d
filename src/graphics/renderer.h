#pragma once
#include "../graphics/opengl.h"
#include "../graphics/color.h"
#include "../structures/hashtable.h"
#include "../structures/coordinates.h"
#include "../util/ext.h"
#include <lua.h>
#include <lua_all.h>

#pragma pack(push, 1)
typedef struct shader_global_t {
    mat4 projection;
    mat4 camera;
} shader_global_t;
#pragma pack(pop)

typedef struct renderer_t {
    // Window
    GLFWwindow *window_handle;
    dimensions_t window_dimensions;
    dimensions_t corrected_dimensions;
    double _last_time;
    double delta_time;
    double fps;

    // Shaders
    hashtable_t shader_table;
    GLuint global;
    mat4 projection;

    // Framebuffer
    GLuint fbo;
    GLuint color_attachment;
    gl_color_t clear_color;

    // Camera
    bool gui;
    mat4 camera_matrix;
    vec2 camera_position;
} renderer_t;
extern renderer_t renderer;

void renderer_init(void);
void renderer_init_window(void);
void renderer_init_framebuffer(void);
void renderer_init_shaders(void);
void renderer_cleanup(void);

bool renderer_loop(void);
void renderer_swap(void);

void renderer_bind(const char *name);
void renderer_update_buffer(void);

void renderer_draw_framebuffer(void);
void renderer_fbo_mouse_position(double *x, double *y);

void renderer_uniform_float(const char *shader, const char *name, float data);
void renderer_uniform_vec3(const char *shader, const char *name, vec3 data);
void renderer_uniform_vec4(const char *shader, const char *name, vec4 data);
void renderer_uniform_mat4(const char *shader, const char *name, mat4 data);

void renderer_set_camera_position(float x, float y);
void renderer_set_camera_center(float x, float y);
void renderer_set_clear_color(color_t color);

void renderer_draw_rectangle(float x, float y, float width, float height, color_t color);
void renderer_draw_text(const char *name, float x, float y, const char *text, color_t color);

/// GLFW window resize event callback.
void _renderer_error_cb(int error_code, const char* description);
void _renderer_framebuffer_resize_cb(unused GLFWwindow *handle, int width, int height);