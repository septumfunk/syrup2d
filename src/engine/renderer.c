//? septumfunk
#include "renderer.h"
#include "window.h"
#include "object_controller.h"
#include "../graphics/shader.h"
#include "../game/game.h"
#include "../win32/msgbox.h"
#include <cglm/cam.h>
#include <cglm/mat4.h>
#include <glad/glad.h>
#include <stdlib.h>
#include <string.h>

renderer_t renderer;

void renderer_init(void) {
    memset(&renderer, 0, sizeof(renderer_t));
    renderer.shader_table = hashtable_string();
    glGenBuffers(1, &renderer.global);
    glBindBuffer(GL_UNIFORM_BUFFER, renderer.global);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(shader_global_t), NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, renderer.global);

    // Framebuffer
    glGenFramebuffers(1, &renderer.fbo);
    glGenTextures(2, &renderer.color_attachment);
    glBindFramebuffer(GL_FRAMEBUFFER, renderer.fbo);

    glBindTexture(GL_TEXTURE_2D, renderer.color_attachment);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window.dimensions[0], window.dimensions[1], 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderer.color_attachment, 0);

    // Callback
    _renderer_framebuffer_resize_cb(window._handle, window.dimensions[0], window.dimensions[1]);
    glfwSetFramebufferSizeCallback(window._handle, _renderer_framebuffer_resize_cb);

    // Projection & Camera
    glm_ortho(0, GAME_WIDTH, GAME_HEIGHT, 0, -255, 255, renderer.projection);
    renderer_set_camera_center(0, 0);

    // Lua
    lua_pushcfunction(object_controller.state, lua_draw_rectangle);
    lua_setglobal(object_controller.state, "draw_rectangle");
}

int lua_draw_rectangle(lua_State *L) {
    // Bounds
    float x = luaL_checknumber(L, 1);
    float y = luaL_checknumber(L, 2);
    float w = luaL_checknumber(L, 3);
    float h = luaL_checknumber(L, 4);

    // Color
    luaL_checktype(L, 5, LUA_TTABLE);
    lua_getfield(L, 5, "r");
    float r = luaL_checknumber(L, -1);
    lua_pop(L, 1);
    lua_getfield(L, 5, "g");
    float g = luaL_checknumber(L, -1);
    lua_pop(L, 1);
    lua_getfield(L, 5, "b");
    float b = luaL_checknumber(L, -1);
    lua_pop(L, 1);
    lua_getfield(L, 5, "a");
    float a = luaL_checknumber(L, -1);
    lua_pop(L, 1);

    renderer_draw_rectangle(x, y, w, h, (color_t){ r, g, b, a });
    return 0;
}

void renderer_cleanup(void) {
    hashtable_delete(&renderer.shader_table);
}

void renderer_bind(const char *name) {
    shader_t *shader;
    if ((shader = hashtable_get(&renderer.shader_table, (void *)name)) == NULL) {
        shader = calloc(1, sizeof(shader_t));
        // Attempt to load shader
        if (shader_load(shader, name).is_error) {
            msgbox_error("Fatal Error", "Shader manager is unable to load shader \"%s\". Are your assets corrupt?", name);
            game_end();
        }
        shader_t *pshader = hashtable_insert(&renderer.shader_table, (void *)name, shader, sizeof(shader_t));
        free(shader);
        shader = pshader;
    }

    // Bind
    glUseProgram(shader->program);
    glBindBuffer(GL_ARRAY_BUFFER, shader->vbo);
    glBindVertexArray(shader->vao);
}

void renderer_update(void) {
    glBindBuffer(GL_UNIFORM_BUFFER, renderer.global);
    shader_global_t global;
    memcpy(global.projection, renderer.projection, sizeof(mat4));
    memcpy(global.camera, renderer.camera_matrix, sizeof(mat4));
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(shader_global_t), &global);
}

void renderer_draw_framebuffer(void) {
    renderer_bind("framebuffer");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindTexture(GL_TEXTURE_2D, renderer.color_attachment);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindFramebuffer(GL_FRAMEBUFFER, renderer.fbo);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
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

void renderer_draw_gui(bool toggle) {
    if (toggle)
        renderer_set_camera_center(0, 0);
    renderer.gui = toggle;
}

void renderer_set_camera_center(float x, float y) {
    if (!renderer.gui) {
        glm_mat4_identity(renderer.camera_matrix);
        glm_translate(renderer.camera_matrix, (vec3){-x + GAME_WIDTH / 2, -y + GAME_HEIGHT / 2, 0});
    }
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
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(2 * sizeof(float)));
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_DYNAMIC_DRAW);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void _renderer_framebuffer_resize_cb(unused GLFWwindow *handle, int width, int height) {
    // Update window
    window.dimensions[0] = width;
    window.dimensions[1] = height;

    // Framebuffer
    renderer_bind("framebuffer");
    float x_mult = 1, y_mult = 1;
    float aspect = ((float)width / (float)GAME_WIDTH) / ((float)height / (float)GAME_HEIGHT);
    if (aspect > 1)
        x_mult = aspect;
    else if (aspect < 1)
        y_mult = aspect;
    float verts[] = {
        -1.0f / x_mult, 1.0f * y_mult, 0.0f, 1.0f, // Top Left
        1.0f / x_mult, 1.0f * y_mult, 1.0f, 1.0f, // Top Right
        -1.0f / x_mult, -1.0f * y_mult, 0.0f, 0.0f, // Bottom Left

        1.0f / x_mult, -1.0f * y_mult, 1.0f, 0.0f, // Bottom Right
        1.0f / x_mult, 1.0f * y_mult, 1.0f, 1.0f, // Top Right
        -1.0f / x_mult, -1.0f * y_mult, 0.0f, 0.0f,  // Bottom Left
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindTexture(GL_TEXTURE_2D, renderer.color_attachment);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

    glViewport(0, 0, width, height);
}
