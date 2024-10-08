//? septumfunk 2024
#include "sprite.h"
#include "../data/fs.h"
#include "../data/stringext.h"
#include "../systems/shaders.h"
#include "../systems/window.h"
#include "../util/ext.h"
#include "../win32/msgbox.h"
#include "shader.h"
#include <glad/glad.h>
#include <stb_image.h>
#include <string.h>

sprite_err_e sprite_from_image(sprite_t *out, const char *path) {
    memset(out, 0, sizeof(sprite_t));
    if (!fs_exists(path))
        return SPRITE_ERR_NOT_FOUND;

    // Load Texture
    int channels;
    out->data.image_data = stbi_load(path, &out->data.width, &out->data.height, &channels, 4);
    if (channels != 4) // NOT RGBA
        return SPRITE_ERR_CHANNEL_COUNT;
    glGenTextures(1, &out->texture);
    glBindTexture(GL_TEXTURE_2D, out->texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, out->data.width, out->data.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, out->data.image_data);
    glGenerateMipmap(GL_TEXTURE_2D);

    return SPRITE_ERR_NONE;
}

void sprite_delete(sprite_t *this) {
    glDeleteTextures(1, &this->texture);
}

void sprite_draw(sprite_t *this, int x, int y, int depth) {
    glBindTexture(GL_TEXTURE_2D, this->texture);
    float verts[] = {
        // Positions         // Tex Coords
        x, y, depth,        0.0f, 0.0f, // Top Left
        x + this->width, y, depth,      1.0f, 0.0f, // Top Right
        x, y + this->height, depth,     0.0f, 1.0f, // Bottom Left

        x + this->width, y + this->height, depth, 1.0f, 1.0f, // Bottom Right
        x + this->width, y, depth,      1.0f, 0.0f, // Top Right
        x, y + this->height, depth,     0.0f, 1.0f  // Bottom Left
    };

    // VAO
    glBindBuffer(GL_ARRAY_BUFFER, shaders[SHADER_SPRITES].vbo);
    glBindVertexArray(shaders[SHADER_SPRITES].vao);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_DYNAMIC_DRAW);

    // Shader
    shader_bind(shaders + SHADER_SPRITES);
    shader_uniform_mat4(shaders + SHADER_SPRITES, SPRITE_LAYOUT_ORTHO, window->ortho);

    // Draw
    glDrawArrays(GL_TRIANGLES, 0, 6);
}
