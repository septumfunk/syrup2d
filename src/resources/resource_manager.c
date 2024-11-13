#include "resource_manager.h"
#include "fs.h"
#include "../util/stringext.h"
#include "../util/log.h"
#include "scene.h"
#include "sprite.h"
#include "cursor.h"
#include <GLFW/glfw3.h>
#include <lauxlib.h>
#include <lua.h>
#include <stdlib.h>
#include <string.h>

resource_manager_t resource_manager;

void resource_manager_init(bool garbage_collecter) {
    log_header("Initializing Resources");
    memset(&resource_manager, 0, sizeof(resource_manager_t));
    resource_manager.gc = garbage_collecter;
    resource_manager.scene_table = hashtable_string();
    resource_manager.sprite_table = hashtable_string();
    resource_manager.cursor_table = hashtable_arbitrary(sizeof(mouse_cursors_e));

    resource_manager.folder = NULL;
    resource_manager.current_scene = NULL;

    result_t res = resource_manager_load_metadata("resources");
    if (res.is_error) {
        result_discard(res);
        res = resource_manager_load_metadata("editor");
        panic(res);
        log_info("Loaded Metadata 'editor/meta.sym'");
        return;
    } else log_info("Loaded Metadata 'resources/meta.sym'");
}

void resource_manager_cleanup(void) {
    hashtable_delete(&resource_manager.sprite_table);
    hashtable_delete(&resource_manager.cursor_table);
}

result_t resource_manager_load_metadata(const char *folder) {
    free(resource_manager.folder);
    resource_manager.folder = _strdup(folder);

    char *path = format(META_PATH, folder);
    if (!fs_exists(path))
        return result_error("DataNotFoundErr", "The game's resources could not be located. Please verify the integrity of your copy of the game.");

    fs_size_t size;
    result_t res;
    char *buffer;
    if ((res = fs_load_checksum(path, &buffer, &size)).is_error) {
        if (result_match(res, "ChecksumCorruptError"))
            return result_error("CorruptGameDataError", "Game data is corrupted. The engine can't start!");
        return res;
    }

    char *head = buffer;
    int len;

    memset(&resource_manager.metadata, 0, sizeof(resource_manager.metadata));
    resource_manager.metadata.version = *(float *)head;
    head += sizeof(float);

    len = strlen(head) + 1;
    resource_manager.metadata.title = calloc(1, len);
    strcpy(resource_manager.metadata.title, head);
    head += len;

    resource_manager.metadata.width = *(uint16_t *)head;
    head += sizeof(uint16_t);

    resource_manager.metadata.height = *(uint16_t *)head;
    head += sizeof(uint16_t);

    resource_manager.metadata.window_scale = *(uint8_t *)head;
    head += sizeof(uint8_t);

    free(path);
    free(buffer);

    return result_no_error();
}

void resource_manager_save_metadata(const char *folder) {
    if (resource_manager.metadata.title == NULL)
        return;

    int len =
      sizeof(float)
    + strlen(resource_manager.metadata.title) + 1
    + sizeof(uint16_t)
    + sizeof(uint16_t)
    + sizeof(uint8_t);

    char *buffer, *head;
    head = buffer = calloc(1, len);

    *(float *)head = META_VERSION;
    head += sizeof(float);
    strcpy(head, resource_manager.metadata.title);
    head += strlen(resource_manager.metadata.title) + 1;
    *(uint16_t *)head = resource_manager.metadata.width;
    head += sizeof(uint16_t);
    *(uint16_t *)head = resource_manager.metadata.height;
    head += sizeof(uint16_t);
    *(uint8_t *)head = resource_manager.metadata.window_scale;
    head += sizeof(uint8_t);

    char *path = format("%s/game.syr", folder);

    result_t res = fs_save_checksum(path, buffer, len);
    panic(res);

    free(path);
    free(buffer);
}

scene_t *resource_manager_scene(const char *name) {
    scene_t *scn;
    if ((scn = hashtable_get(&resource_manager.scene_table, (void *)name)) == NULL) {
        scn = calloc(1, sizeof(scene_t));
        // Attempt to load scene
        result_t res;
        if ((res = scene_load(name, scn)).is_error) {
            log_info(res.description);
            result_discard(res);
        } else log_info("Loaded sprite '%s'.", name);

        scene_t *pscn = hashtable_insert(&resource_manager.scene_table, (void *)name, scn, sizeof(scene_t));
        free(scn);
        scn = pscn;
    }
    scn->last_accessed = glfwGetTime();

    return scn;
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
                panic(result_error("Fatal Error", "Resource manager is unable to load default sprite. Are your assets corrupt?"));
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

GLFWcursor *resource_manager_cursor(mouse_cursors_e cursor) {
    GLFWcursor **c = hashtable_get(&resource_manager.cursor_table, &cursor);
    if (!c) {
        GLFWcursor *c_ptr = glfwCreateStandardCursor(cursor);
        c = hashtable_insert(&resource_manager.cursor_table, &cursor, &c_ptr, sizeof(GLFWcursor *));
    }
    return *c;
}

void resource_manager_clean(void) {
    if (!resource_manager.gc || (resource_manager.sprite_table.pair_count == 0 && resource_manager.scene_table.pair_count == 0))
        return;

    // Clean Scenes
    uint32_t count = 0;
    pair_t **pairs = hashtable_pairs(&resource_manager.scene_table, &count);
    for (pair_t **pair = pairs; pair < pairs + count; ++pair) {
        scene_t *scn = (scene_t *)(*pair)->value;
        if (scn->active && resource_manager.current_scene != scn && glfwGetTime() - scn->last_accessed >= SCENE_DECAY_TIME) {
            scene_delete(scn);
            log_info("Unloaded scene '%s'.", (*pair)->key);
            hashtable_remove(&resource_manager.scene_table, (*pair)->key);
        }
    }
    free(pairs);

    // Clean Sprites
    count = 0;
    pairs = hashtable_pairs(&resource_manager.sprite_table, &count);
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
