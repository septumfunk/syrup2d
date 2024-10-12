//? septumfunk 2024
#include "game.h"
#include "../data/stringext.h"
#include "../engine/window.h"
#include "../engine/renderer.h"
#include "../engine/sprite_manager.h"
#include "../engine/object_controller.h"
#include "../engine/controller.h"
#include <math.h>
#include <string.h>

void game_init(void) {
    object_controller_init();
    window_init(GAME_TITLE, GAME_WIDTH * 6, GAME_HEIGHT * 6);
    renderer_init();
    sprite_manager_init(GARBAGE_COLLECTOR_ENABLED);
    controller_init();

    game_loop();
}

void game_end(void) {
    window_cleanup();
    sprite_manager_cleanup();
    object_controller_cleanup();
    exit(0);
}

void game_loop(void) {
    float x = 0, y = 0;
    result_t res = object_controller_new("player", 0, 0);
    if (error_is(res, "ObjectCodeError"))
        error_fatal(res);
    res = object_controller_new("pizza", -10, -10);
    if (error_is(res, "ObjectCodeError"))
        error_fatal(res);

    while (window_loop()) {
        // Update
        object_controller_update();
        renderer_update();
        controller_reset();

        // Draw
        object_controller_draw();
        sprite_manager_clean();
        renderer_draw_framebuffer();
        window_swap();
    }
}
