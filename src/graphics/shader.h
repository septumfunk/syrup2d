#pragma once
#include "opengl.h"
#include "../structures/result.h"
#include "../structures/hashtable.h"

#define VERTEX_SHADER_PATH "%s/.syrup/shaders/%s.vert"
#define FRAGMENT_SHADER_PATH "%s/.syrup/shaders/%s.frag"
#define GLOBAL_SHADER_PATH "%s/.syrup/shaders/global.glsl"

/// OpenGL shader program, conveniently wrapped
typedef struct shader_t {
    GLuint program;
    GLuint vertex;
    GLuint fragment;
    GLuint vbo, vao;
    hashtable_t uniform_table;
} shader_t;

result_t shader_load(shader_t *out, const char *name);
void shader_delete(shader_t *this);