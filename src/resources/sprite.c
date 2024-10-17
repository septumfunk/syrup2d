#include "sprite.h"
#include "../resources/fs.h"
#include "../util/stringext.h"
#include "../util/crypto.h"
#include <glad/glad.h>
#include <stb_image.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

result_t sprite_load(sprite_t *out, const char *name) {
    // Name
    out->name = calloc(1, strlen(name) + 1);
    strcpy(out->name, name);
    char *path = format(SPRITE_PATH, name);

    if (!fs_exists(path)) {
        free(path);
        path = format(SPRITE_ENGINE_PATH, name);
    }

    // Load file
    char *buffer;
    fs_size_t size;
    result_t res = fs_load_checksum(path, &buffer, &size);
    char *head = buffer;

    if (res.is_error)
        return res;
    if (size <= sizeof(spritedata_t))
        return result_error("SpriteCorruptError", "Sprite data appears to be corrupt.");

    out->data.version = *(float *)head;
    head += sizeof(float);
    out->data.width = *(uint16_t *)head;
    head += sizeof(uint16_t);
    out->data.height = *(uint16_t *)head;
    head += sizeof(uint16_t);
    out->data.frame_count = *(uint8_t *)head;
    head += sizeof(uint8_t);
    out->data.frame_delay = *(uint8_t *)head;
    head += sizeof(uint8_t);

    out->image_data = calloc(1, out->data.width * out->data.height * 4 /* (RGBA) */);
    memcpy(out->image_data, head, out->data.width * out->data.height * 4 /* (RGBA) */);

    glGenTextures(1, &out->texture);
    glBindTexture(GL_TEXTURE_2D, out->texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, out->data.width, out->data.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, out->image_data);
    glGenerateMipmap(GL_TEXTURE_2D);

    free(path);
    return result_no_error();
}

result_t sprite_from_image(sprite_t *out, const char *name) {
    // Name
    out->name = calloc(1, strlen(name) + 1);
    strcpy(out->name, name);
    char *path = format(SPRITE_IMPORT_PATH, name);

    // Load
    if (!fs_exists(path))
        return result_error("SpriteNotFoundError", "The requested sprite '%s' was not able to be found.", name);

    // Load Texture
    int width, height, channels;
    out->image_data = stbi_load(path, &width, &height, &channels, 4);
    out->data.width = width;
    out->data.height = height;

    if (out->image_data == NULL)
        return result_error("SpriteReadError", "The requested sprite '%s' data was not able to be read.", name);
    glGenTextures(1, &out->texture);
    glBindTexture(GL_TEXTURE_2D, out->texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, out->data.width, out->data.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, out->image_data);
    glGenerateMipmap(GL_TEXTURE_2D);

    free(path);
    return result_no_error();
}

result_t sprite_save(sprite_t *this) {
    // Size
    int img_size = this->data.width * this->data.height * 4 /* (RGBA) */;
    int buffer_size = sizeof(spritedata_t) + img_size;

    // Copy to buffer
    char *buffer = calloc(1, buffer_size);
    char *head = buffer;

    *(float *)head = this->data.version;
    head += sizeof(float);
    *(uint16_t *)head = this->data.width;
    head += sizeof(uint16_t);
    *(uint16_t *)head = this->data.height;
    head += sizeof(uint16_t);
    *(uint8_t *)head = this->data.frame_count;
    head += sizeof(uint8_t);
    *(uint8_t *)head = this->data.frame_delay;
    head += sizeof(uint8_t);

    memcpy(head, this->image_data, img_size);

    // Write
    char *path = format(SPRITE_PATH, this->name);
    result_t res = fs_save_checksum(path, buffer, buffer_size);
    if (res.is_error)
        return res;
    free(path);

    free(buffer);
    return result_no_error();
}

void sprite_delete(sprite_t *this) {
    glDeleteTextures(1, &this->texture);
}

void sprite_draw(sprite_t *this, float x, float y, uint8_t frame_index) {
    sprite_draw_tint(this, x, y, frame_index, (color_t) { 255, 255, 255, 255 });
}

void sprite_draw_tint(sprite_t *this, float x, float y, uint8_t frame_index, color_t tint) {
    gl_color_t t = color_to_gl(tint);
    glBindTexture(GL_TEXTURE_2D, this->texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    float frame_width = 1.0f / this->data.frame_count;
    float offset = frame_index * frame_width;

    float true_width = this->data.frame_count == 0 ? this->data.width : this->data.width / this->data.frame_count;
    float verts[] = {
        x, y, offset, 0.0f, t.r, t.g, t.b, t.a, // Top Left
        x + true_width, y, offset + frame_width, 0.0f, t.r, t.g, t.b, t.a, // Top Right
        x, y + this->data.height, offset, 1.0f, t.r, t.g, t.b, t.a, // Bottom Left

        x + true_width, y + this->data.height, offset + frame_width, 1.0f, t.r, t.g, t.b, t.a, // Bottom Right
        x + true_width, y,  offset + frame_width, 0.0f, t.r, t.g, t.b, t.a, // Top Right
        x, y + this->data.height, offset, 1.0f, t.r, t.g, t.b, t.a,  // Bottom Left
    };

    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_DYNAMIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}
