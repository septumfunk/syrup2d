// septumfunk 2024

#include "game.h"
#include "../systems/controller.h"
#include "../systems/window.h"
#include "../win32/msgbox.h"

void game_init(void) {
    window_init(GAME_TITLE, GAME_WIDTH, GAME_HEIGHT);
    controller_init();
    game_loop();
}

void game_end(void) {
    if (controller) controller_cleanup();
    if (window) window_cleanup();
}

void game_loop(void) {
    while (window_loop()) {
        if (is_key_pressed(KEY_ESCAPE)) {
            game_end();
            break;
        }

        controller_reset();
        window_twap();
    }
}
