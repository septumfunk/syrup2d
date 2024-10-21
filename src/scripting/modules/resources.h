#pragma once
#include "modules.h"
#include <lua.h>

int api_resources_import_sprite(lua_State *L);

int api_resources_get_game_data(lua_State *L);
int api_resources_set_game_data(lua_State *L);