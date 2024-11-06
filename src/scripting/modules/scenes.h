#pragma once
#include "modules.h"
#include <lua.h>

int api_scenes_instances(lua_State *L);
int api_scenes_add_instance(lua_State *L);
int api_scenes_delete_instance(lua_State *L);
int api_scenes_set_instance_type(lua_State *L);
int api_scenes_move_instance(lua_State *L);
