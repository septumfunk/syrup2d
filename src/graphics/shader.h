//? septumfunk 2024
#pragma once
#include "opengl.h"
#include "../data/result.h"
#include "../data/hashtable.h"

#define VERTEX_SHADER_PATH "resources/shaders/%s.vert"
#define FRAGMENT_SHADER_PATH "resources/shaders/%s.frag"

/// OpenGL shader program, conveniently wrapped
typedef struct shader_t {
    GLuint program;
    GLuint vertex;
    GLuint fragment;
    GLuint vbo, vao;
    hashtable_t uniform_table;
} shader_t;

/// Load shader via name, see VERTEX_SHADER_PATH and FRAGMENT_SHADER_PATH for locations
result_t shader_load(shader_t *out, const char *name);
/// Delete shader and clean up resources
void shader_delete(shader_t *this);
/// Bind OpenGL to specified shader
void shader_bind(shader_t *this);