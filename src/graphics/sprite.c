//? septumfunk 2024
#include "sprite.h"
#include "../data/fs.h"
#include "../game/game.h"
#include "../win32/msgbox.h"
#include <stb_image.h>

sprite_t sprite_load(const char *path) {
    sprite_t sprite;

    if (!fs_exists(path)) {
        msgbox_error("Unable to Locate Texture", "Failed to locate texture \"%s\"", path);
        game_end();
    }

    // Load Texture
    unsigned char *data = stbi_load(path, &sprite.width, &sprite.height, &sprite.channels, 0);
    glGenTextures(1, &sprite.texture);
    glBindTexture(GL_TEXTURE_2D, sprite.texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, sprite.width, sprite.height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    return sprite;
}
