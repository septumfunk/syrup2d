#include "engine.h"
#include "graphics/color.h"
#include "graphics/renderer.h"
#include "input/keyboard_mouse.h"
#include "resources/resource_manager.h"
#include "resources/scene.h"
#include "scripting/scripting_api.h"
#include "util/log.h"
#include <GLFW/glfw3.h>
#include <lauxlib.h>
#include <lua.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

void engine_start() {
    resource_manager_init(GARBAGE_COLLECTOR_ENABLED);
    renderer_init();
    keyboard_mouse_init();
    scripting_api_init();

    log_header("Starting Game '%s'", resource_manager.metadata.title);

    scene_t *scene = resource_manager_scene("SceneEditorB1");
    scene_spawn_instances(scene);

    while (renderer_loop()) {
        // Update
        scripting_api_update();

        // Draw
        scripting_api_draw();
        resource_manager_clean();
        keyboard_mouse_reset();
        renderer_swap();
    }
}

void engine_stop(void) {
    scripting_api_cleanup();
    renderer_cleanup();
    resource_manager_cleanup();
    exit(0);
}