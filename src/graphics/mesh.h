// septumfunk 2024

#include "../data/vector.h"
#include "shader.h"
#include <cglm/types.h>

#pragma pack(push, 1)
typedef struct vertex_t {
    float x, y, z;
} vertex_t;

typedef struct tri_t {
    vertex_t v1, v2, v3;
} tri_t;
#pragma pack(pop)
