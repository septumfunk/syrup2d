#pragma once
#include "modules.h"
#include <lua.h>

int api_input_is_key_pressed(lua_State *L);
int api_input_is_key_down(lua_State *L);
int api_input_is_mouse_button_pressed(lua_State *L);
int api_input_is_mouse_button_down(lua_State *L);
int api_input_keyboard_string(lua_State *L);
int api_input_set_cursor(lua_State *L);