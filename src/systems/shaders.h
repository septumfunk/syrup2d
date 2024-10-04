//? septumfunk 2024
#pragma once
#include "../graphics/shader.h"

typedef enum shaders_e {
    SHADER_SPRITES,

    SHADER_COUNT,
} shaders_e;
extern shader_t shaders[SHADER_COUNT];

void shaders_init(void);