//? septumfunk 2024
#pragma once
#include "opengl.h"
#include "../data/result.h"

#define SPRITE_PATH_IMAGE "resources/import/%s.png"
#define SPRITE_PATH_DATA "resources/sprites/%s.syrspr"
#define SPRITE_VERSION 1.1f
#define SPRITE_DECAY_TIME 3.0f

/// Inner sprite data that gets written and read from disk
typedef struct spritedata_t {
    float version;
    uint16_t width;
    uint16_t height;
    uint8_t frame_count;
    uint8_t frame_delay;
    uint8_t channels;
} spritedata_t;

/// Sprite struct compatible with OpenGL drawing
typedef struct sprite_t {
    char *name;
    GLuint texture;
    struct spritedata_t data;

    int ref_count;
    float decay;
    unsigned char *image_data;
} sprite_t;

/// Load a sprite by its name directly
result_t sprite_load(sprite_t *out, const char *name);
/// Load a sprite from an image path
result_t sprite_from_image(sprite_t *out, const char *name);
/// Load a sprite from an image path
result_t sprite_save(sprite_t *this);
/// Destroy and clean up a sprite's resources
void sprite_delete(sprite_t *this);

/// Draw a sprite to the screen
void sprite_draw(sprite_t *this, float x, float y, uint8_t frame_index);