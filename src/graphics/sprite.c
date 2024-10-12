//? septumfunk 2024
#include "sprite.h"
#include "../data/fs.h"
#include "../data/stringext.h"
#include "../data/crypto.h"
#include "../engine/renderer.h"
#include <glad/glad.h>
#include <stb_image.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

result_t sprite_load(sprite_t *out, const char *name) {
    // Name
    out->name = calloc(1, strlen(name) + 1);
    strcpy(out->name, name);
    char *path = format(SPRITE_PATH_DATA, name);

    // Load file
    char *buffer;
    fs_size_t size;
    result_t res = fs_load_checksum(path, &buffer, &size);
    char *head = buffer;

    if (error_is(res, "ChecksumCorruptError"))
        return error("SpriteChecksumError", "Sprite data doesn't match its checksum. It is likely corrupt.");
    if (res.is_error)
        return res;

    if (size <= sizeof(spritedata_t))
        return error("SpriteCorruptError", "Sprite data appears to be corrupt.");
    size -= 4;

    out->data.version = *(float *)head;
    head += sizeof(float);
    out->data.width = *(int *)head;
    head += sizeof(int);
    out->data.height = *(int *)head;
    head += sizeof(int);
    out->data.channels = *(int *)head;
    head += sizeof(int);
    out->data.frame_count = *(int *)head;
    head += sizeof(int);
    out->data.frame_delay = *(int *)head;
    head += sizeof(int);

    out->image_data = calloc(1, out->data.width * out->data.height * out->data.channels);
    memcpy(out->image_data, head, out->data.width * out->data.height * out->data.channels);

    glGenTextures(1, &out->texture);
    glBindTexture(GL_TEXTURE_2D, out->texture);
    glTexImage2D(GL_TEXTURE_2D, 0, out->data.channels == 3 ? GL_RGB : GL_RGBA, out->data.width, out->data.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, out->image_data);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Reference counter
    out->decay = SPRITE_DECAY_TIME;

    free(path);
    return no_error();
}

result_t sprite_from_image(sprite_t *out, const char *name) {
    // Name
    out->name = calloc(1, strlen(name) + 1);
    strcpy(out->name, name);
    char *path = format(SPRITE_PATH_IMAGE, name);

    // Load
    if (!fs_exists(path))
        return error("SpriteNotFoundError", "The requested sprite was not able to be found on the filesystem.");

    // Load Texture
    out->image_data = stbi_load(path, &out->data.width, &out->data.height, &out->data.channels, 4);
    if (out->image_data == NULL)
        return error("SpriteReadError", "The requested sprite's data was not able to be read.");
    if (out->data.channels != 3 && out->data.channels != 4)
        return error("SpriteChannelError", "The requested image is not in an RGB or RGBA format.");
    glGenTextures(1, &out->texture);
    glBindTexture(GL_TEXTURE_2D, out->texture);
    glTexImage2D(GL_TEXTURE_2D, 0, out->data.channels == 3 ? GL_RGB : GL_RGBA, out->data.width, out->data.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, out->image_data);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Reference counter
    out->decay = SPRITE_DECAY_TIME;

    sprite_save(out);

    free(path);
    return no_error();
}

result_t sprite_save(sprite_t *this) {
    // Size
    int img_size = this->data.width * this->data.height * this->data.channels;
    int buffer_size = sizeof(spritedata_t) + img_size;

    // Copy to buffer
    char *buffer = calloc(1, buffer_size);
    char *head = buffer;

    *(float *)head = this->data.version;
    head += sizeof(float);
    *(int *)head = this->data.width;
    head += sizeof(int);
    *(int *)head = this->data.height;
    head += sizeof(int);
    *(int *)head = this->data.channels;
    head += sizeof(int);
    *(int *)head = this->data.frame_count;
    head += sizeof(int);
    *(int *)head = this->data.frame_delay;
    head += sizeof(int);

    memcpy(head, this->image_data, img_size);

    // Write
    char *path = format(SPRITE_PATH_DATA, this->name);
    result_t res = fs_save_checksum(path, buffer, buffer_size);
    if (res.is_error)
        return res;

    free(buffer);
    return no_error();
}

void sprite_delete(sprite_t *this) {
    glDeleteTextures(1, &this->texture);
}

void sprite_draw(sprite_t *this, float x, float y) {
    glBindTexture(GL_TEXTURE_2D, this->texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    float verts[] = {
        x, y,        0.0f, 0.0f, // Top Left
        x + this->data.width, y,      1.0f, 0.0f, // Top Right
        x, y + this->data.height,     0.0f, 1.0f, // Bottom Left

        x + this->data.width, y + this->data.height, 1.0f, 1.0f, // Bottom Right
        x + this->data.width, y,      1.0f, 0.0f, // Top Right
        x, y + this->data.height,     0.0f, 1.0f  // Bottom Left
    };

    renderer_bind("sprite");
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_DYNAMIC_DRAW);

    // Draw
    glDrawArrays(GL_TRIANGLES, 0, 6);
}
