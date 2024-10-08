//? septumfunk 2024
#include "game.h"
#include "../graphics/sprite.h"
#include "../data/hashtable.h"
#include "../data/stringext.h"
#include "../win32/msgbox.h"
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
    float x = 0, y = 0;
    while (window_loop()) {
        if (is_key_pressed(KEY_ESCAPE)) {
            game_end();
            break;
        }

        if (is_key_down(KEY_A)) x -= 0.05;
        if (is_key_down(KEY_D)) x += 0.05;
        if (is_key_down(KEY_S)) y += 0.05;
        if (is_key_down(KEY_W)) y -= 0.05;

        sprite_draw(hashtable_get(&table, "vap"), x, y, 0);

        controller_reset();
        window_swap();
    }
}
