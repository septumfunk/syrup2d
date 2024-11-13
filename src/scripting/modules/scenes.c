#include "scenes.h"
#include "../../resources/resource_manager.h"
#include <lauxlib.h>
#include <lua.h>
#include <stdint.h>

scripting_function_t api_scenes_functions[] = {
    { "instances", api_scenes_instances },
    { "add_instance", api_scenes_add_instance },
    { "delete_instance", api_scenes_delete_instance },
    { "set_instance_type", api_scenes_set_instance_type },
    { "move_instance", api_scenes_move_instance },
};

__attribute__((constructor)) void api_scenes_init(void) {
    scripting_modules[SCRIPTING_MODULES_SCENES] = (scripting_module_t) {
        .name = "scenes",
        .function_count = sizeof(api_scenes_functions) / sizeof(scripting_function_t),
        .functions = api_scenes_functions,
    };
}

int api_scenes_instances(lua_State *L) {
    scene_t *scene = resource_manager_scene(luaL_checkstring(L, 1));

    lua_newtable(L);
    uint32_t index = 1;
    instance_t *head = scene->instances;
    while (head) {
        lua_newtable(L);

        lua_pushstring(L, head->type);
        lua_setfield(L, -2, "type");

        lua_pushnumber(L, head->x);
        lua_setfield(L, -2, "x");

        lua_pushnumber(L, head->y);
        lua_setfield(L, -2, "y");

        lua_pushnumber(L, head->z);
        lua_setfield(L, -2, "z");

        lua_rawseti(L, -2, index++);
        head = head->next;
    }

    return 1;
}

int api_scenes_add_instance(lua_State *L) {
    scene_t *scene = resource_manager_scene(luaL_checkstring(L, 1));

    scene_add_instance(scene, luaL_checkstring(L, 1), luaL_checknumber(L, 2), luaL_checknumber(L, 3), luaL_checknumber(L, 4), luaL_checkstring(L, 5));
    scene_save(scene, scene->name);

    return 0;
}

int api_scenes_delete_instance(lua_State *L) {
    scene_t *scene = resource_manager_scene(luaL_checkstring(L, 1));

    int i = luaL_checkinteger(L, 1);
    if (i < 0)
        return 0;

    scene_delete_instance(scene, i);
    scene_save(scene, scene->name);

    return 0;
}

int api_scenes_set_instance_type(lua_State *L) {
    scene_t *scene = resource_manager_scene(luaL_checkstring(L, 1));

    int i = luaL_checkinteger(L, 1);
    if (i < 0)
        return 0;

    scene_set_instance_type(scene, i, luaL_checkstring(L, 2));
    scene_save(scene, scene->name);

    return 0;
}

int api_scenes_move_instance(lua_State *L) {
    scene_t *scene = resource_manager_scene(luaL_checkstring(L, 1));

    int i = luaL_checkinteger(L, 1);
    if (i < 0)
        return 0;

    scene_move_instance(scene, i, luaL_checknumber(L, 2), luaL_checknumber(L, 3), luaL_checknumber(L, 4));
    scene_save(scene, scene->name);

    return 0;
}

