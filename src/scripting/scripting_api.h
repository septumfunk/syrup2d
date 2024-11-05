#pragma once
#include "object_manager.h"
#include "../structures/result.h"
#include <lua_all.h>
#include <stdint.h>

#define OBJECT_PATH "%s/objects/%s.lua"
#define OBJECT_ENGINE_PATH "%s/.syrup/objects/%s.lua"

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

result_t scripting_api_push(const char *type, float x, float y);
result_t scripting_api_create(const char *type, float x, float y);
void scripting_api_delete(uint32_t id);

void scripting_api_copy_table(void);
void scripting_api_dump_stack(void);

int api_internal_base_start(lua_State *L);
int api_internal_base_update(lua_State *L);
int api_internal_base_draw(lua_State *L);
int api_internal_base_draw_ui(lua_State *L);
int api_internal_base_clean_up(lua_State *L);