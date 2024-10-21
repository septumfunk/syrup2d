#include "graphics.h"
#include "../../graphics/renderer.h"
#include "../../resources/resource_manager.h"
#include <lauxlib.h>
#include <lua.h>
#include <lua_all.h>

scripting_function_t api_graphics_functions[] = {
    { "draw_rectangle", api_graphics_draw_rectangle },
    { "draw_sprite", api_graphics_draw_sprite },
    { "draw_sprite_pro",  api_graphics_draw_sprite_pro },
    { "draw_text", api_graphics_draw_text },
};

__attribute__((constructor)) void api_graphics_init(void) {
    scripting_modules[SCRIPTING_MODULE_GRAPHICS] = (scripting_module_t) {
        .name = "graphics",
        .function_count = sizeof(api_graphics_functions) / sizeof(scripting_function_t),
        .functions = api_graphics_functions,
    };
}

int api_graphics_draw_rectangle(lua_State *L) {
    renderer_bind("rectangle");

    luaL_checktype(L, 5, LUA_TTABLE);
    lua_getfield(L, 5, "r");
    float r = luaL_checknumber(L, -1);
    lua_pop(L, 1);
    lua_getfield(L, 5, "g");
    float g = luaL_checknumber(L, -1);
    lua_pop(L, 1);
    lua_getfield(L, 5, "b");
    float b = luaL_checknumber(L, -1);
    lua_pop(L, 1);
    lua_getfield(L, 5, "a");
    float a = luaL_checknumber(L, -1);
    lua_pop(L, 1);

    renderer_draw_rectangle(luaL_checknumber(L, 1), luaL_checknumber(L, 2), luaL_checknumber(L, 3), luaL_checknumber(L, 4), (color_t) {
        .r = r,
        .g = g,
        .b = b,
        .a = a,
    });
    return 0;
}

int api_graphics_draw_sprite(lua_State *L) {
    renderer_bind("sprite");
    sprite_draw(resource_manager_sprite(luaL_checkstring(L, 1)), luaL_checknumber(L, 2), luaL_checknumber(L, 3), luaL_checkinteger(L, 4));
    return 0;
}

int api_graphics_draw_sprite_pro(lua_State *L) {
    renderer_bind("sprite");

    luaL_checktype(L, 8, LUA_TTABLE);
    lua_getfield(L, 8, "r");
    float r = luaL_checknumber(L, -1);
    lua_pop(L, 1);
    lua_getfield(L, 8, "g");
    float g = luaL_checknumber(L, -1);
    lua_pop(L, 1);
    lua_getfield(L, 8, "b");
    float b = luaL_checknumber(L, -1);
    lua_pop(L, 1);
    lua_getfield(L, 8, "a");
    float a = luaL_checknumber(L, -1);
    lua_pop(L, 1);

    sprite_draw_pro(
        resource_manager_sprite(luaL_checkstring(L, 1)),
        luaL_checknumber(L, 2),
        luaL_checknumber(L, 3),
        luaL_checknumber(L, 4),
        luaL_checknumber(L, 5),
        luaL_checknumber(L, 6),
        luaL_checknumber(L, 7),
        (color_t) {
            .r = r,
            .g = g,
            .b = b,
            .a = a,
        }
    );
    return 0;
}

int api_graphics_draw_text(lua_State *L) {
    renderer_bind("sprite");

    luaL_checktype(L, 5, LUA_TTABLE);
    lua_getfield(L, 5, "r");
    float r = luaL_checknumber(L, -1);
    lua_pop(L, 1);
    lua_getfield(L, 5, "g");
    float g = luaL_checknumber(L, -1);
    lua_pop(L, 1);
    lua_getfield(L, 5, "b");
    float b = luaL_checknumber(L, -1);
    lua_pop(L, 1);
    lua_getfield(L, 5, "a");
    float a = luaL_checknumber(L, -1);
    lua_pop(L, 1);

    renderer_draw_text(luaL_checkstring(L, 1), luaL_checknumber(L, 2), luaL_checknumber(L, 3), luaL_checkstring(L, 4), (color_t) {
        .r = r,
        .g = g,
        .b = b,
        .a = a,
    });
    return 0;
}
