// septumfunk 2024
#ifndef SHADER_H
#define SHADER_H

#include "opengl.h"

#define VERTEX_SHADER_PATH "data/shaders/%s.v"
#define FRAGMENT_SHADER_PATH "data/shaders/%s.f"

typedef struct shader_t {
    GLuint program;
    GLuint vertex;
    GLuint fragment;
} shader_t;

typedef enum shader_err_e {
    SHADER_ERR_NONE,
    SHADER_ERR_NOT_FOUND,
    SHADER_ERR_LOAD,
    SHADER_ERR_LINK,
} shader_err_e;

shader_err_e shader_load(shader_t *out, const char *name);
void shader_delete(shader_t *this);
void shader_bind(shader_t *this);

#endif // SHADER_H
