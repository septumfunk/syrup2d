//? septumfunk 2024
#pragma once
#include "opengl.h"

#define SPRITE_PATH "assets/sprites/%s.mspr"
#define SPRITE_VERSION 1.0f

/// Inner sprite data that gets written and read from disk
typedef struct spritedata_t {
    float version;
    int width;
    int height;
    int frame_delay;
    unsigned char *image_data;
} spritedata_t;

/// Sprite struct compatible with OpenGL drawing
typedef struct sprite_t {
    GLuint texture;
    int frame;
    int depth;
    struct spritedata_t data;
} sprite_t;

/// Sprite errors enum
typedef enum sprite_err_e {
    SPRITE_ERR_NONE, /// No error in loading sprites
    SPRITE_ERR_NOT_FOUND, /// Unable to locate sprite in the filesystem
    SPRITE_ERR_CHANNEL_COUNT, /// Sprite has an incorrect amount of channels
} sprite_err_e;

/// Sprite layout enum
typedef enum sprite_layout_e {
    SPRITE_LAYOUT_POSITION,
    SPRITE_LAYOUT_TEXCOORD,
    SPRITE_LAYOUT_ORTHO,
} sprite_layout_e;

/// Load a sprite by its name directly
sprite_err_e sprite_load(sprite_t *out, const char *name);
/// Load a sprite from an image path
sprite_err_e sprite_from_image(sprite_t *out, const char *path);
/// Destroy and clean up a sprite's resources
void sprite_delete(sprite_t *this);

/// Draw a sprite to the screen
void sprite_draw(sprite_t *this, int x, int y, int depth);