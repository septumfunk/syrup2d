//? septumfunk 2024
#include "sprite_manager.h"
#include "window.h"
#include <lauxlib.h>
#include <stdlib.h>
#include <string.h>

sprite_manager_t sprite_manager;

void sprite_manager_init(bool garbage_collecter) {
    memset(&sprite_manager, 0, sizeof(sprite_manager_t));
    sprite_manager.gc = garbage_collecter;
    sprite_manager.table = hashtable_string();
}

void sprite_manager_cleanup(void) {
    hashtable_delete(&sprite_manager.table);
}

sprite_t *sprite_manager_get(const char *name) {
    sprite_t *spr;
    if ((spr = hashtable_get(&sprite_manager.table, (void *)name)) == NULL) {
        spr = calloc(1, sizeof(sprite_t));
        // Attempt to load sprite
        if (sprite_load(spr, name).is_error
        && sprite_from_image(spr, name).is_error
        && sprite_load(spr, "default").is_error
        && sprite_from_image(spr, "default").is_error)
            panic(error("Fatal Error", "Sprite manager is unable to load default sprite. Are your assets corrupt?"));
        sprite_t *pspr = hashtable_insert(&sprite_manager.table, (void *)name, spr, sizeof(sprite_t));
        free(spr);
        spr = pspr;
    }
    spr->ref_count++;
    return spr;
}

void sprite_manager_draw(const char *name, float x, float y) {
    sprite_draw(sprite_manager_get(name), x, y);
}

int lua_draw_sprite(lua_State *L) {
    const char *name = luaL_checkstring(L, 1);
    double x = luaL_checknumber(L, 2);
    double y = luaL_checknumber(L, 3);
    sprite_manager_draw(name, x, y);
    return 0;
}

void sprite_manager_clean(void) {
    if (!sprite_manager.gc || sprite_manager.table.pair_count == 0)
        return;
    uint32_t count = 0;
    pair_t **pairs = hashtable_pairs(&sprite_manager.table, &count);
    for (pair_t **pair = pairs; pair < pairs + count; ++pair) {
        sprite_t *spr = (sprite_t *)(*pair)->value;
        if (spr->ref_count == 0) {
            spr->decay -= window.delta_time;
            if (spr->decay <= 0) {
                sprite_delete(spr);
                hashtable_remove(&sprite_manager.table, (*pair)->key);
            }
        } else {
            spr->decay = SPRITE_DECAY_TIME;
            spr->ref_count = 0;
        }
    }
    free(pairs);
}