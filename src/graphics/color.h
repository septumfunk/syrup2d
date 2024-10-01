// septumfunk 2024
#ifndef COLOR_H
#define COLOR_H

#include <stdint.h>

// Standard RGBA color
typedef struct color_t {
    uint8_t r, g, b, a;
} color_t;

// Color compatible with OpenGL
typedef struct gl_color_t {
    float r, g, b, a;
} gl_color_t;

gl_color_t color_to_gl(color_t color);
color_t gl_to_color(gl_color_t color);

#endif // COLOR_H
