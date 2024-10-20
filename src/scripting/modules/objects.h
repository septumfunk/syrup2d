#pragma once
#include <lua_all.h>

int api_objects_new(lua_State *L);
int api_objects_delete(lua_State *L);
int api_objects_get(lua_State *L);
int api_objects_get_all(lua_State *L);