// septumfunk
#pragma once
#include <stdint.h>

#define DEFAULT_GAME_TITLE "Syrup Engine"
#define DEFAULT_GAME_WIDTH 256
#define DEFAULT_GAME_HEIGHT 144

/// Data structure for a game's important information
typedef struct game_data_t {
    char *title;
    uint32_t game_width, game_height;
} game_data_t;
extern game_data_t game_data;

/// Initialize the engine and engine
void game_init(void);
/// Stop the game loop and all engine
void game_end(void);
/// Start the game loop
void game_loop(void);

/// Load game data from a file (or create it, if it doesn't exist)
void game_load_data(const char *folder);
