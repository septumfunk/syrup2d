#include "renderer.h"
#include "../resources/resource_manager.h"
#include "../util/log.h"
#include "shader.h"
#include "../structures/result.h"
#include "../util/msgbox.h"
#include <GLFW/glfw3.h>
#include <cglm/cam.h>
#include <cglm/mat4.h>
#include <glad/glad.h>
#include <lauxlib.h>
#include <lua.h>
#include <stdlib.h>
#include <string.h>

renderer_t renderer;

void renderer_init(void) {
    memset(&renderer, 0, sizeof(renderer_t));
    renderer_init_window();
    renderer_init_shaders();
    renderer_init_framebuffer();
}

void renderer_init_window(void) {
    if (!glfwInit())
        panic(result_error("GLFWInitializeError", "Failed to initialize GLFW."));

    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    if ((renderer.window_handle = glfwCreateWindow(
        resource_manager.game_data.width * resource_manager.game_data.window_scale,
        resource_manager.game_data.height * resource_manager.game_data.window_scale,
        resource_manager.game_data.title, NULL, NULL)) == NULL)
        panic(result_error("GLFWWindowError", "Failed to open GLFW window."));

    // Callbacks
    glfwSetErrorCallback(_renderer_error_cb);
    glfwSetFramebufferSizeCallback(renderer.window_handle, _renderer_framebuffer_resize_cb);

    // Load GLAD
    glfwMakeContextCurrent(renderer.window_handle);
    if (!gladLoadGL())
        panic(result_error("GLADInitializeError", "Failed to initialize GLAD."));

    // Depth and Blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glfwSetWindowSizeLimits(
        renderer.window_handle,
        resource_manager.game_data.width,
        resource_manager.game_data.height,
        resource_manager.game_data.width * 8,
        resource_manager.game_data.height * 8
    );

    // Icon
    sprite_t *ico_spr = resource_manager_sprite("icon");
    GLFWimage icon = {
        .width = ico_spr->data.width,
        .height = ico_spr->data.height,
        .pixels = ico_spr->image_data,
    };
    glfwSetWindowIcon(renderer.window_handle, 1, &icon);

    // Ready
    glViewport(0, 0, resource_manager.game_data.width, resource_manager.game_data.height);
    glfwSwapInterval(1);
    glfwShowWindow(renderer.window_handle);
}

void renderer_init_shaders(void) {
    renderer.shader_table = hashtable_string();

    glm_ortho(0, resource_manager.game_data.width, resource_manager.game_data.height, 0, -255, 255, renderer.projection);
    renderer_set_camera_center(0, 0);
    glGenBuffers(1, &renderer.global);
    glBindBuffer(GL_UNIFORM_BUFFER, renderer.global);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(shader_global_t), NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, renderer.global);

    renderer_bind("rectangle");
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(2 * sizeof(float)));

    renderer_bind("sprite");
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(4 * sizeof(float)));
}

void renderer_init_framebuffer(void) {
    _renderer_framebuffer_resize_cb(
        renderer.window_handle,
        resource_manager.game_data.width * resource_manager.game_data.window_scale,
        resource_manager.game_data.height * resource_manager.game_data.window_scale
    );

    glGenFramebuffers(1, &renderer.fbo);
    glGenTextures(2, &renderer.color_attachment);
    glBindFramebuffer(GL_FRAMEBUFFER, renderer.fbo);

    glBindTexture(GL_TEXTURE_2D, renderer.color_attachment);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, renderer.window_dimensions.width, renderer.window_dimensions.height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderer.color_attachment, 0);

    renderer_bind("framebuffer");
    float verts[] = {
        -1.0f, 1.0f, 0.0f, 1.0f, // Top Left
        1.0f, 1.0f, 1.0f, 1.0f, // Top Right
        -1.0f, -1.0f, 0.0f, 0.0f, // Bottom Left

        1.0f, -1.0f, 1.0f, 0.0f, // Bottom Right
        1.0f, 1.0f, 1.0f, 1.0f, // Top Right
        -1.0f, -1.0f, 0.0f, 0.0f,  // Bottom Left
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    renderer_set_clear_color((color_t) { 0, 0, 0, 255 });
}

void renderer_cleanup(void) {
    hashtable_delete(&renderer.shader_table);
    glfwSetWindowShouldClose(renderer.window_handle, true);
    if (renderer.window_handle)
        glfwDestroyWindow(renderer.window_handle);
}

bool renderer_loop(void) {
    for (GLenum err = glGetError(); err != GL_NO_ERROR; err = glGetError())
        log_warn("OpenGL Error: %d", err);

    renderer._last_time = glfwGetTime();
    renderer_update_buffer();

    return !glfwWindowShouldClose(renderer.window_handle);
}

void renderer_swap(void) {
    renderer_draw_framebuffer();
    glfwSwapBuffers(renderer.window_handle);
    glfwPollEvents();

    // Minimized
    while (glfwGetWindowAttrib(renderer.window_handle, GLFW_ICONIFIED))
        glfwPollEvents();

    renderer.delta_time = glfwGetTime() - renderer._last_time;
    renderer.fps = 1.0f / renderer.delta_time;
}

void renderer_bind(const char *name) {
    shader_t *shader;
    if ((shader = hashtable_get(&renderer.shader_table, (void *)name)) == NULL) {
        shader = calloc(1, sizeof(shader_t));
        panic(shader_load(shader, name));
        shader_t *pshader = hashtable_insert(&renderer.shader_table, (void *)name, shader, sizeof(shader_t));
        free(shader);
        shader = pshader;
    }

    glUseProgram(shader->program);
    glBindBuffer(GL_ARRAY_BUFFER, shader->vbo);
    glBindVertexArray(shader->vao);
}

void renderer_update_buffer(void) {
    glBindBuffer(GL_UNIFORM_BUFFER, renderer.global);
    shader_global_t global;
    memcpy(global.projection, renderer.projection, sizeof(mat4));
    if (renderer.gui)
        memcpy(global.camera, GLM_MAT4_IDENTITY, sizeof(mat4));
    else
        memcpy(global.camera, renderer.camera_matrix, sizeof(mat4));
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(shader_global_t), &global);
}

void renderer_draw_framebuffer(void) {
    renderer_bind("framebuffer");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0, 0, 0, 255);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindTexture(GL_TEXTURE_2D, renderer.color_attachment);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindFramebuffer(GL_FRAMEBUFFER, renderer.fbo);
    glClearColor(renderer.clear_color.r, renderer.clear_color.g, renderer.clear_color.b, renderer.clear_color.a);
    glClear(GL_COLOR_BUFFER_BIT);
}

void renderer_uniform_float(const char *shader, const char *name, float data) {
    renderer_bind(shader);
    shader_t *s = hashtable_get(&renderer.shader_table, (void *)shader);

    GLuint *lPtr = hashtable_get(&s->uniform_table, (void *)name);
    int location;
    if (lPtr == NULL) {
        location = glGetUniformLocation(s->program, name);
        hashtable_insert(&s->uniform_table, (void *)name, &location, sizeof(GLuint));
    } else
        location = *lPtr;
    glUniform1f(location, data);
}

void renderer_uniform_vec3(const char *shader, const char *name, vec3 data) {
    renderer_bind(shader);
    shader_t *s = hashtable_get(&renderer.shader_table, (void *)shader);

    GLuint *lPtr = hashtable_get(&s->uniform_table, (void *)name);
    GLuint location = (lPtr == NULL ? glGetUniformLocation(s->program, name) : *lPtr);
    glUniform3fv(location, 1, &data[0]);
}

void renderer_uniform_vec4(const char *shader, const char *name, vec4 data) {
    renderer_bind(shader);
    shader_t *s = hashtable_get(&renderer.shader_table, (void*)shader);

    GLuint *lPtr = hashtable_get(&s->uniform_table, (void *)name);
    GLuint location = (lPtr == NULL ? glGetUniformLocation(s->program, name) : *lPtr);
    glUniform4fv(location, 1, &data[0]);
}

void renderer_uniform_mat4(const char *shader, const char *name, mat4 data) {
    renderer_bind(shader);
    shader_t *s = hashtable_get(&renderer.shader_table, (void*)shader);

    GLuint *lPtr = hashtable_get(&s->uniform_table, (void*)name);
    GLuint location = (lPtr == NULL ? glGetUniformLocation(s->program, name) : *lPtr);
    glUniformMatrix4fv(location, 1, false, &data[0][0]);
}

void renderer_set_camera_center(float x, float y) {
    glm_mat4_identity(renderer.camera_matrix);
    glm_translate(renderer.camera_matrix, (vec3){-x + resource_manager.game_data.width / 2, -y + resource_manager.game_data.height / 2, 0});
}

int lua_camera_center(lua_State *L) {
    renderer_set_camera_center(luaL_checknumber(L, 1), luaL_checknumber(L, 2));
    return 0;
}

void renderer_set_clear_color(color_t color) {
    renderer.clear_color = color_to_gl(color);
}

void renderer_fbo_mouse_position(double *x, double *y) {
    double mx, my;
    glfwGetCursorPos(renderer.window_handle, &mx, &my);

    double dx = (double)renderer.window_dimensions.width / resource_manager.game_data.width;
    double dy = (double)renderer.window_dimensions.height / resource_manager.game_data.height;
    double aspect = dx / dy;
    *x = mx / dx;
    *y = my / dy;

    if (aspect > 1)
        *x = (mx - (renderer.window_dimensions.width - dy * resource_manager.game_data.width) / 2) / dy;
    if (aspect < 1)
        *y = (my - (renderer.window_dimensions.height - dx * resource_manager.game_data.height) / 2) / dx;
}

void renderer_draw_rectangle(float x, float y, float width, float height, color_t color) {
    gl_color_t c = color_to_gl(color);
    float verts[] = {
        x, y, c.r, c.g, c.b, c.a, // Top Left
        x + width, y, c.r, c.g, c.b, c.a, // Top Right
        x, y + height, c.r, c.g, c.b, c.a, // Bottom Left

        x + width, y + height, c.r, c.g, c.b, c.a, // Bottom Right
        x + width, y, c.r, c.g, c.b, c.a, // Top Right
        x, y + height, c.r, c.g, c.b, c.a,   // Bottom Left
    };
    renderer_bind("rectangle");
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_DYNAMIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void renderer_draw_text(const char *name, float x, float y, const char *text, color_t color) {
    sprite_t *spr = resource_manager_sprite(name);
    int charwidth = spr->data.width / spr->data.frame_count;
    float x_offset = 0;

    for (const char *c = text; *c != '\0'; ++c) {
        char idx = *c;
        if (idx == '\n') {
            y += spr->data.height;
            x_offset = 0;
            continue;
        }
        if (idx > 126 || idx < 32)
            idx = '?';
        sprite_draw_pro(resource_manager_sprite(name), x + x_offset, y, 1, 1, 0, idx - 32, color);
        x_offset += charwidth;
    }
}

void _renderer_framebuffer_resize_cb(unused GLFWwindow *handle, int width, int height) {
    renderer.window_dimensions.width = width;
    renderer.window_dimensions.height = height;

    float framebuffer_aspect = (float)resource_manager.game_data.width / resource_manager.game_data.height;
    float window_aspect = (float)width / height;

    int viewport_width, viewport_height;
    int x_offset = 0, y_offset = 0;

    if (window_aspect > framebuffer_aspect) {
        viewport_height = height;
        viewport_width = (int)(framebuffer_aspect * height);
        x_offset = (width - viewport_width) / 2;  // Center horizontally
    } else {
        viewport_width = width;
        viewport_height = (int)(width / framebuffer_aspect);
        y_offset = (height - viewport_height) / 2;  // Center vertically
    }

    glViewport(x_offset, y_offset, viewport_width, viewport_height);
    glBindTexture(GL_TEXTURE_2D, renderer.color_attachment);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
}

void _renderer_error_cb(int error_code, const char* description) {
    msgbox_warn("OpenGL Error", "OpenGL Error:\n%s\n\nError Code: %d", description, error_code);
}
