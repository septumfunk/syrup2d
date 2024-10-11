//? septumfunk 2024
#pragma once
#include "../data/hashtable.h"
#include "../data/result.h"
#include <lua.h>
#include <lua_all.h>

#define OBJECT_PATH "resources/objects/%s"

/// A gameobject as a node in a linked list of the same type
typedef struct gameobject_t {
    uint32_t id;
    char *type;
    float depth;
    struct gameobject_t *previous, *next;
} gameobject_t;

/// Object controller system responsible for loading in objects
/// and managing their lifetime, update calls and draw calls.
typedef struct object_controller_t {
    uint32_t current_id;
    hashtable_t object_table;
    lua_State *state;
} object_controller_t;
extern object_controller_t object_controller;

/// Initialize the object controller system.
void object_controller_init(void);
/// Clean up after the object controller system.
void object_controller_cleanup(void);

/// Call all object update logic.
void object_controller_update(void);
/// Call all object draw logic.
void object_controller_draw(void);
/// Intantiate an object by name.
result_t object_controller_new(const char *name);

/// Push an object's table onto the stack
void object_controller_get(uint32_t id);
/// Push an object's field onto the stack
void object_controller_get_field(uint32_t id, const char *name);

/* Lua Function */

/// Get an object's id by type.
int lua_object(lua_State *L);