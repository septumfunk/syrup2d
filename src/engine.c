#include "engine.h"
#include "graphics/color.h"
#include "graphics/renderer.h"
#include "input/keyboard_mouse.h"
#include "resources/resource_manager.h"
#include "scripting/scripting_api.h"
#include <GLFW/glfw3.h>
#include <lauxlib.h>
#include <lua.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

void engine_start() {
    resource_manager.game_data = (syrup_config_t) {
        .fixed_size = false,
        .width = 1280,
        .height = 720,
        .window_scale = 1,
        .title = "Syrup Editor",
        .version = CONFIG_VERSION,
    };
    resource_manager_save_game_data("resources");
    resource_manager_init(GARBAGE_COLLECTOR_ENABLED);
    renderer_init();
    keyboard_mouse_init();
    scripting_api_init();

    result_t res = scripting_api_create("editor/ui_controller", 0, 0);
    panic(res);

    while (renderer_loop()) {
        // Update
        scripting_api_update();

        // Draw
        scripting_api_draw();
        resource_manager_clean();
        keyboard_mouse_reset();
        renderer_swap();
    }

    resource_manager_save_game_data("resources");
}

void engine_stop(void) {
    scripting_api_cleanup();
    renderer_cleanup();
    resource_manager_cleanup();
    exit(0);
}