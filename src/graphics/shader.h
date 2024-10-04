//? septumfunk 2024
#pragma once
#include "opengl.h"

#define VERTEX_SHADER_PATH "data/shaders/%s.v"
#define FRAGMENT_SHADER_PATH "data/shaders/%s.f"

/// OpenGL shader program, conveniently wrapped
typedef struct shader_t {
    GLuint program;
    GLuint vertex;
    GLuint fragment;
} shader_t;

/// Possible errors returned while loading shaders
typedef enum shader_err_e {
    SHADER_ERR_NONE, /// No error occured while loading shaders
    SHADER_ERR_NOT_FOUND, /// Unable to locate shader file
    SHADER_ERR_LOAD, /// Unable to read from shader file
    SHADER_ERR_LINK, /// Unable to link vertex and fragment shaders
} shader_err_e;

/// Load shader via name, see VERTEX_SHADER_PATH and FRAGMENT_SHADER_PATH for locations
shader_err_e shader_load(shader_t *out, const char *name);
/// Delete shader and clean up resources
void shader_delete(shader_t *this);
/// Bind OpenGL to specified shader
void shader_bind(shader_t *this);
