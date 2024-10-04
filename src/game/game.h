// septumfunk
#pragma once

#define GAME_TITLE "The Death of Eric Cartman"
#define GAME_WIDTH 1280
#define GAME_HEIGHT 720

/// Initialize the engine and systems
void game_init(void);
/// Stop the game loop and all systems
void game_end(void);
/// Start the game loop
void game_loop(void);
