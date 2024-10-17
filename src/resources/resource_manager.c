#include "resource_manager.h"
#include "fs.h"
#include "../util/stringext.h"
#include "../util/log.h"
#include "sprite.h"
#include <lauxlib.h>
#include <lua.h>
#include <stdlib.h>
#include <string.h>

resource_manager_t resource_manager;

void resource_manager_init(bool garbage_collecter) {
    memset(&resource_manager, 0, sizeof(resource_manager_t));
    resource_manager.gc = garbage_collecter;
    resource_manager.sprite_table = hashtable_string();
    resource_manager_load_game_data("resources");
}

void resource_manager_cleanup(void) {
    hashtable_delete(&resource_manager.sprite_table);
}

void resource_manager_load_game_data(const char *folder) {
    char *path = format("%s/game.syr", folder);
    if (!fs_exists(path))
        panic(result_error("DataNotFoundErr", "The game's resources could not be located. Please restore your resources or point the engine to them using the '-open' flag."));

    fs_size_t size;
    result_t res;
    char *buffer;
    if ((res = fs_load_checksum(path, &buffer, &size)).is_error) {
        if (result_match(res, "ChecksumCorruptError"))
            panic(result_error("CorruptGameDataError", "Game data is corrupted. The engine can't start!"));
        panic(res);
    }

    char *head = buffer;
    int len;

    memset(&resource_manager.game_data, 0, sizeof(resource_manager.game_data));
    resource_manager.game_data.version = *(float *)head;
    head += sizeof(float);

    len = strlen(head) + 1;
    resource_manager.game_data.title = calloc(1, len);
    strcpy(resource_manager.game_data.title, head);
    head += len;

    resource_manager.game_data.width = *(uint16_t *)head;
    head += sizeof(uint16_t);

    resource_manager.game_data.height = *(uint16_t *)head;
    head += sizeof(uint16_t);

    resource_manager.game_data.window_scale = *(uint8_t *)head;
    head += sizeof(uint8_t);

    free(path);
    free(buffer);
}

void resource_manager_save_game_data(const char *folder) {
    if (resource_manager.game_data.title == NULL)
        return;

    int len =
      sizeof(float)
    + strlen(resource_manager.game_data.title) + 1
    + sizeof(uint16_t)
    + sizeof(uint16_t)
    + sizeof(uint8_t);

    char *buffer, *head;
    head = buffer = calloc(1, len);

    *(float *)head = CONFIG_VERSION;
    head += sizeof(float);
    strcpy(head, resource_manager.game_data.title);
    head += strlen(resource_manager.game_data.title) + 1;
    *(uint16_t *)head = resource_manager.game_data.width;
    head += sizeof(uint16_t);
    *(uint16_t *)head = resource_manager.game_data.height;
    head += sizeof(uint16_t);
    *(uint8_t *)head = resource_manager.game_data.window_scale;
    head += sizeof(uint8_t);

    char *path = format("%s/game.syr", folder);
    panic(fs_save_checksum(path, buffer, len));
    free(path);
    free(buffer);
}

sprite_t *resource_manager_sprite(const char *name) {
    sprite_t *spr;
    if ((spr = hashtable_get(&resource_manager.sprite_table, (void *)name)) == NULL) {
        spr = calloc(1, sizeof(sprite_t));
        // Attempt to load sprite
        result_t res;
        if ((res = sprite_load(spr, name)).is_error) {
            log_info(res.description);
            result_discard(res);
            if (sprite_load(spr, "default").is_error)
                panic(result_error("Fatal Error", "Sprite manager is unable to load default sprite. Are your assets corrupt?"));
        } else
            log_info("Loaded sprite '%s'.", name);

        sprite_t *pspr = hashtable_insert(&resource_manager.sprite_table, (void *)name, spr, sizeof(sprite_t));
        free(spr);
        spr = pspr;
    }
    spr->last_drawn = glfwGetTime();

    return spr;
}

result_t resource_manager_import_sprite(const char *name, uint8_t frame_count, uint8_t frame_delay) {
    sprite_t spr;
    result_t res;
    if ((res = sprite_from_image(&spr, name)).is_error) {
        log_warn(res.description);
        return res;
    }

    spr.data.frame_count = frame_count;
    spr.data.frame_delay = frame_delay;

    if (spr.data.width % spr.data.frame_count != 0) {
        sprite_delete(&spr);
        return result_error("SpriteFrameError", "The sprite's width is not evenly divisible by the provided frame count.");
    }

    if (hashtable_get(&resource_manager.sprite_table, (void *)name))
        hashtable_remove(&resource_manager.sprite_table, (void *)name);
    hashtable_insert(&resource_manager.sprite_table, (void *)name, &spr, sizeof(sprite_t));

    sprite_save(&spr);

    return result_no_error();
}

void resource_manager_clean(void) {
    if (!resource_manager.gc || resource_manager.sprite_table.pair_count == 0)
        return;
    uint32_t count = 0;
    pair_t **pairs = hashtable_pairs(&resource_manager.sprite_table, &count);
    for (pair_t **pair = pairs; pair < pairs + count; ++pair) {
        sprite_t *spr = (sprite_t *)(*pair)->value;
        if (glfwGetTime() - spr->last_drawn >= SPRITE_DECAY_TIME) {
            sprite_delete(spr);
            log_info("Unloaded sprite '%s'.", (*pair)->key);
            hashtable_remove(&resource_manager.sprite_table, (*pair)->key);
        }
    }
    free(pairs);
}
