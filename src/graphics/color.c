#include "color.h"
#include <cglm/util.h>
#include <math.h>
#include <stdlib.h>

gl_color_t color_to_gl(color_t color) {
    return (gl_color_t) {
        .r = color.r/255.0f,
        .g = color.g/255.0f,
        .b = color.b/255.0f,
        .a = color.a/255.0f,
    };
}

color_t gl_to_color(gl_color_t color) {
    return (color_t) {
        .r = floor(glm_min(255, color.r * 255)),
        .g = floor(glm_min(255, color.g * 255)),
        .b = floor(glm_min(255, color.b * 255)),
        .a = floor(glm_min(255, color.a * 255)),
    };
}
