//? septumfunk 2024
#pragma once
#include "shader.h"

extern shader_t *sprite_shader;

/// Sprite struct compatible with OpenGL drawing
typedef struct sprite_t {
    GLuint texture;
    int width, height, channels;
} sprite_t;

/// Load sprite shader


sprite_t sprite_load(const char *path);