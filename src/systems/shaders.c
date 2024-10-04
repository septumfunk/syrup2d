//? septumfunk 2024
#include "shaders.h"
#include "../win32/msgbox.h"
#include "../game/game.h"

static const char *const _shader_names[SHADER_COUNT] = {
    "sprite" // SHADER_SPRITE
};
shader_t shaders[SHADER_COUNT];

void shaders_init(void) {
    for (int i = 0; i < SHADER_COUNT; ++i) {
        if (shader_load(shaders + i, _shader_names[i]) != SHADER_ERR_NONE) {
            msgbox_error("Fatal Error", "Failed to load shader \"%s\"\nGame will now terminate.", _shader_names[i]);
            game_end();
        }
    }
}