//? septumfunk 2024
#include "engine.h"
#include "../data/stringext.h"
#include "../data/fs.h"
#include "../win32/files.h"
#include "../engine/window.h"
#include "../engine/renderer.h"
#include "../engine/sprite_manager.h"
#include "../engine/object_controller.h"
#include "../engine/controller.h"
#include <GLFW/glfw3.h>
#include <lua.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

game_data_t game_data;
engine_t engine;

void engine_start(const char *folder) {
    engine_load_game_data(folder);
    object_controller_init();
    window_init(game_data.title, game_data.game_width * game_data.window_scale, game_data.game_height * game_data.window_scale);
    renderer_init();
    sprite_manager_init(GARBAGE_COLLECTOR_ENABLED);
    controller_init();

    // Lua
    lua_pushcfunction(object_controller.state, lua_game_end);
    lua_setglobal(object_controller.state, "game_end");

    // Temp
    sprite_manager_import("ui_font", 95, 0);

    result_t res = object_controller_new("ui/debug_menu", 0, 0);
    if (res.is_error)
        error_warn(res);

    while (window_loop()) {
        // Configure delta time
        engine._last_time = glfwGetTime();

        // Update
        object_controller_update();
        renderer_update_buffer();

        // Draw
        object_controller_draw();
        sprite_manager_clean();
        renderer_draw_framebuffer();
        controller_reset();
        window_swap();

        // Finish delta time
        engine.delta_time = glfwGetTime() - engine._last_time;
        engine.fps = 1.0f / engine.delta_time;
    }

    engine_save_game_data();
}

void engine_stop(void) {
    window_cleanup();
    sprite_manager_cleanup();
    object_controller_cleanup();
    free(game_data.title);
    exit(0);
}

int lua_game_end(unused lua_State *L) {
    engine_stop();
    return 0;
}

void engine_load_game_data(const char *folder) {
    char *path = format("%s/game.syr", folder);
    if (!fs_exists(path))
        panic(error("DataNotFoundErr", "The game's resources could not be located. Please restore your resources or point the engine to them using the '-open' flag."));

    fs_size_t size;
    result_t res;
    char *buffer;
    if ((res = fs_load_checksum(path, &buffer, &size)).is_error) {
        if (error_is(res, "ChecksumCorruptError"))
            panic(error("CorruptGameDataError", "Game data is corrupted. The engine can't start! Delete it and allow the engine to autogenerate if you are unable to restore it."));
        panic(res);
    }

    char *head = buffer;
    int len;

    game_data.version = *(float *)head;
    head += sizeof(float);

    len = strlen(head) + 1;
    game_data.title = calloc(1, len);
    strcpy(game_data.title, head);
    head += len;

    game_data.game_width = *(uint16_t *)head;
    head += sizeof(uint16_t);

    game_data.game_height = *(uint16_t *)head;
    head += sizeof(uint16_t);

    game_data.window_scale = *(uint8_t *)head;
    head += sizeof(uint8_t);

    free(path);
    free(buffer);
}

void engine_save_game_data(void) {
    if (game_data.title == NULL)
        return;

    int len =
      sizeof(float)
    + strlen(game_data.title) + 1
    + sizeof(uint16_t)
    + sizeof(uint16_t)
    + sizeof(uint8_t);

    char *buffer, *head;
    head = buffer = calloc(1, len);

    *(float *)head = ENGINE_VERSION;
    head += sizeof(float);
    strcpy(head, game_data.title);
    head += strlen(game_data.title) + 1;
    *(uint16_t *)head = game_data.game_width;
    head += sizeof(uint16_t);
    *(uint16_t *)head = game_data.game_height;
    head += sizeof(uint16_t);
    *(uint8_t *)head = game_data.window_scale;
    head += sizeof(uint8_t);

    panic(fs_save_checksum("resources/game.syr", buffer, len));
    free(buffer);
}
