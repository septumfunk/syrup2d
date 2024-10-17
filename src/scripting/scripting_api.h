#pragma once
#include "object_manager.h"
#include "../structures/result.h"
#include <lua.h>
#include <stdint.h>

typedef struct scripting_api_t {
    uint32_t current_id;
    lua_State *state;
    object_manager_t manager;
} scripting_api_t;
extern scripting_api_t scripting_api;

void scripting_api_init(void);
void scripting_api_init_state(void);
void scripting_api_init_globals(void);
void scripting_api_init_modules(void);
void scripting_api_cleanup(void);

void scripting_api_update(void);
void scripting_api_update_globals(void);
void scripting_api_draw(void);

result_t scripting_api_create(const char *type, float x, float y);