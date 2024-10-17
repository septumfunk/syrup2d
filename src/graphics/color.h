#pragma once
#include <stdint.h>

/// Standard RGBA color
typedef struct color_t {
    uint8_t r, g, b, a;
} color_t;

/// Color compatible with OpenGL
typedef struct gl_color_t {
    float r, g, b, a;
} gl_color_t;

/// Convert a standard RGBA color into an OpenGL compatible color
gl_color_t color_to_gl(color_t color);
/// Convert an OpenGL compatible color into a standard RGBA color
color_t gl_to_color(gl_color_t color);
