//? septumfunk 2024
#include "shader.h"
#include "../data/stringext.h"
#include "../data/fs.h"
#include <glad/glad.h>
#include <stdlib.h>

result_t load_shader_file(GLuint *out, int type, const char *name) {
    *out = glCreateShader(type);

    // Source
    char *path = format(type == GL_VERTEX_SHADER ? VERTEX_SHADER_PATH : FRAGMENT_SHADER_PATH, name);
    char *source = NULL;
    if (!fs_exists(path))
        return error("ShaderNotFoundError", "The requested shader could not be found on the filesystem.");

    fs_size_t size;
    if (fs_load(path, &source, &size).is_error)
        return error("ShaderNotFoundError", "The requested shader's data could not be read.");

    // Prepend global
    char *global = NULL;
    char *p = format("resources/shaders/global.glsl");
    if (!fs_exists(p))
        panic(error("AssetsCorruptError", "Global shader data is missing. Assets are corrupted."));
    if (fs_load(p, &global, &size).is_error)
        panic(error("AssetsCorruptError", "Global shader data is missing. Assets are corrupted."));
    free(p);

    char *osrs = source;
    source = format(osrs, global);
    free(osrs);
    osrs = source;
    source = format("%s\0", osrs);
    free(global);
    free(osrs);

    // Compile
    glShaderSource(*out, 1, (const GLchar *const *)&source, NULL);
    glCompileShader(*out);

    int success;
    glGetShaderiv(*out, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(*out, 512, NULL, log);
        panic(error("Fatal OpenGL Error", format("Failed to compile shader \"%s.%s\"!\n%s", name, type == GL_VERTEX_SHADER ? "vert" : "frag", log)));
    }

    free(path);
    free(source);
    return no_error();
}

result_t shader_load(shader_t *out, const char *name) {
    result_t result = load_shader_file(&out->vertex, GL_VERTEX_SHADER, name);
    if (result.is_error)
        return result;
    result = load_shader_file(&out->fragment, GL_FRAGMENT_SHADER, name);
    if (result.is_error)
        return result;

    out->program = glCreateProgram();
    glAttachShader(out->program, out->vertex);
    glAttachShader(out->program, out->fragment);
    glLinkProgram(out->program);

    int success;
    glGetProgramiv(out->program, GL_LINK_STATUS, &success);
    if (!success) {
        char log[512];
        glGetProgramInfoLog(out->program, 512, NULL, log);
        panic(error("Fatal OpenGL Error", format("Failed to link shader \"%s\"!\n%s", name, log)));
    }

    // VAO & VBO
    glGenVertexArrays(1, &out->vao);
    glGenBuffers(1, &out->vbo);

    // Uniform table
    out->uniform_table = hashtable_string();

    return no_error();
}

void shader_bind(shader_t *this) {
    glUseProgram(this->program);
}

void shader_uniform_mat4(shader_t *this, GLuint location, mat4 data) {
    shader_bind(this);
    glUniformMatrix4fv(location, 1, false, &data[0][0]);
}
