//? septumfunk 2024
#pragma once
#include "../graphics/opengl.h"
#include "../graphics/color.h"
#include "../data/hashtable.h"
#include "../util/ext.h"
#include <lua_all.h>

#pragma pack(push, 1)
typedef struct shader_global_t {
    mat4 projection;
    mat4 camera;
} shader_global_t;
#pragma pack(pop)

/// The engine's renderer system.
/// Processes draw calls, dynamically loads shaders, manages camera, and manages the framebuffer.
typedef struct renderer_t {
    // Shading
    hashtable_t shader_table;
    GLuint global;
    mat4 projection;

    // Framebuffer
    GLuint fbo;
    GLuint color_attachment;

    // Camera
    bool gui;
    mat4 camera_matrix;
} renderer_t;
extern renderer_t renderer;

/// Initialize the renderer system.
void renderer_init(void);
/// Clean up after the renderer system.
void renderer_cleanup(void);

/// Bind to a shader by its name.
/// The renderer will load shaders which are not found in the table.
/// Binds to VAO & VBO as well.
void renderer_bind(const char *name);
/// Update renderer and shader data.
void renderer_update(void);
/// Draw shader renderer's framebuffer to the screen.
void renderer_draw_framebuffer(void);

/// Set a float uniform.
void renderer_uniform_float(const char *shader, const char *name, float data);
/// Set a vec3 uniform.
void renderer_uniform_vec3(const char *shader, const char *name, vec3 data);
/// Set a vec4 uniform.
void renderer_uniform_vec4(const char *shader, const char *name, vec4 data);
/// Set a mat4 uniform.
void renderer_uniform_mat4(const char *shader, const char *name, mat4 data);

/// Enter or exit gui drawing mode.
void renderer_draw_gui(bool toggle);

/// Set the renderer's camera position.
void renderer_set_camera_center(float x, float y);

/// Draw a colored rectangle.
void renderer_draw_rectangle(float x, float y, float width, float height, color_t color);
int lua_draw_rectangle(lua_State *L);

/// GLFW window resize event callback.
void _renderer_framebuffer_resize_cb(unused GLFWwindow *handle, int width, int height);