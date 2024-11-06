#pragma once
#include "modules.h"
#include <lua.h>

int api_graphics_draw_rectangle(lua_State *L);
int api_graphics_draw_sprite(lua_State *L);
int api_graphics_draw_sprite_pro(lua_State *L);
int api_graphics_sprite_dimensions(lua_State *L);
int api_graphics_draw_text(lua_State *L);

int api_graphics_get_camera_position(lua_State *L);
int api_graphics_set_camera_position(lua_State *L);
int api_graphics_get_camera_center(lua_State *L);
int api_graphics_set_camera_center(lua_State *L);
