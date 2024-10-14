// septumfunk
#pragma once
#include <lua.h>
#include <stdint.h>

#define ENGINE_VERSION 0.1
#define DEFAULT_GAME_TITLE "Syrup Engine"
#define DEFAULT_RESOURCE_FOLDER "resources"
#define DEFAULT_GAME_WIDTH 256
#define DEFAULT_GAME_HEIGHT 144

/// Data structure for a game's important information.
typedef struct game_data_t {
    float version;
    char *title;
    uint16_t game_width, game_height;
    uint8_t window_scale;
} game_data_t;
extern game_data_t game_data;

/// Engine data structure
typedef struct engine_t {
    double _last_time;
    double delta_time;
    double fps;
} engine_t;
extern engine_t engine;

/// Start the engine and all systems.
void engine_start(const char *folder);
/// Stop the engine and all systems.
void engine_stop(void);
int lua_game_end(lua_State *L);

/// Load game data from a folder.
void engine_load_game_data(const char *folder);
/// Load game data from a folder.
void engine_save_game_data(void);
