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
    window_init(GAME_TITLE, GAME_WIDTH * 6, GAME_HEIGHT * 6);
    renderer_init();
    object_controller_init();
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
    result_t res = object_controller_new("player");
    if (error_is(res, "ObjectCodeError"))
        error_fatal(res);
    res = object_controller_new("pizza");
    if (error_is(res, "ObjectCodeError"))
        error_fatal(res);
    while (window_loop()) {
        // Update
        renderer_update();
        if (is_key_pressed(KEY_ESCAPE)) {
            game_end();
            break;
        }

        if (is_key_down(KEY_A)) x -= 100 * window.delta_time;
        if (is_key_down(KEY_D)) x += 100 * window.delta_time;
        if (is_key_down(KEY_S)) y += 100 * window.delta_time;
        if (is_key_down(KEY_W)) y -= 100 * window.delta_time;
        renderer_set_camera_center(x, y);

        // Draw
        renderer_draw_rectangle(10, 10, 40, 40, (color_t) { 150, 150, 255, 75 });

        object_controller_update();
        renderer_draw_framebuffer();
        sprite_manager_clean();
        controller_reset();
        window_swap();
    }
}
