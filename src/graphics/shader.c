//? septumfunk 2024
#include "shader.h"
#include "../data/stringext.h"
#include "../data/fs.h"
#include "../win32/msgbox.h"
#include "../util/ext.h"
#include <glad/glad.h>

GLuint load_shader_file(int type, const char *name) {
    GLuint shader = glCreateShader(type);

    // Source
    char *path = format(type == GL_VERTEX_SHADER ? VERTEX_SHADER_PATH : FRAGMENT_SHADER_PATH, name);
    char *source = NULL;
    if (!fs_exists(path))
        goto failure;

    if (fs_read(path, &source) != FS_ERR_NONE)
        goto failure;

    glShaderSource(shader, 1, (const GLchar *const *)&source, NULL);
    glCompileShader(shader);

    // Error Checking
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(shader, 512, NULL, log);
        msgbox_error("OpenGL Error", "Failed to compile shader \"%s.%s\"!\n\nError:\n%s", name, type == GL_VERTEX_SHADER ? "vert" : "frag", log);
        goto failure;
    }

    free(path);
    free(source);
    return shader;

failure:
    free(path);
    free(source);
    return 0;
}

shader_err_e shader_load(shader_t *out, const char *name) {
    out->vertex = load_shader_file(GL_VERTEX_SHADER, name);
    if (out->vertex < 1)
        return SHADER_ERR_LOAD;
    out->fragment = load_shader_file(GL_FRAGMENT_SHADER, name);
    if (out->fragment < 1)
        return SHADER_ERR_LOAD;

    out->program = glCreateProgram();
    glAttachShader(out->program, out->vertex);
    glAttachShader(out->program, out->fragment);
    glLinkProgram(out->program);

    int success;
    glGetProgramiv(out->program, GL_LINK_STATUS, &success);
    if (!success) {
        char log[512];
        glGetProgramInfoLog(out->program, 512, NULL, log);
        msgbox_error("OpenGL Error", "Failed to link shader \"%s\"!\n\nError:\n%s", name, log);
        return SHADER_ERR_LINK;
    }

    // VAO & VBO
    glGenVertexArrays(1, &out->vao);
    glGenBuffers(1, &out->vbo);

    return SHADER_ERR_NONE;
}

void shader_bind(shader_t *this) {
    glUseProgram(this->program);
}

void shader_uniform_mat4(shader_t *this, GLuint location, mat4 data) {
    shader_bind(this);
    glUniformMatrix4fv(location, 1, false, &data[0][0]);
}
