//? septumfunk 2024
#include "game.h"
#include "../systems/window.h"
#include "../systems/shaders.h"
#include "../systems/controller.h"

void game_init(void) {
    window_init(GAME_TITLE, GAME_WIDTH, GAME_HEIGHT);
    shaders_init();
    controller_init();
    game_loop();
}

void game_end(void) {
    if (controller) controller_cleanup();
    if (window) window_cleanup();
    exit(0);
}

void game_loop(void) {
    while (window_loop()) {
        if (is_key_pressed(KEY_ESCAPE)) {
            game_end();
            break;
        }

        controller_reset();
        window_swap();
    }
}
