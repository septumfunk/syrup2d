#include "shader.h"
#include "../util/stringext.h"
#include "../resources/fs.h"
#include "../resources/resource_manager.h"
#include <glad/glad.h>
#include <stdlib.h>

result_t load_shader_file(GLuint *out, int type, const char *name) {
    *out = glCreateShader(type);

    // Source
    char *path = format(type == GL_VERTEX_SHADER ? VERTEX_SHADER_PATH : FRAGMENT_SHADER_PATH, resource_manager.folder, name);
    char *source = NULL;
    if (!fs_exists(path))
        return result_error("ShaderNotFoundError", "The requested shader could not be found on the filesystem.");

    fs_size_t size;
    result_t res;
    if ((res = fs_load(path, &source, &size)).is_error)
        return res;
    source = realloc(source, size + 1); // Null terminator

    // Prepend global
    char *global = NULL;
    free(path);
    path = format(GLOBAL_SHADER_PATH, resource_manager.folder);
    if (!fs_exists(path) || fs_load(path, &global, &size).is_error)
        panic(result_error("AssetsCorruptError", "Global shader data is missing. Assets are corrupted."));

    char *osrs = source;
    source = format(osrs, global);
    free(osrs);
    free(global);

    // Compile
    glShaderSource(*out, 1, (const GLchar *const *)&source, NULL);
    glCompileShader(*out);

    int success;
    glGetShaderiv(*out, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(*out, 512, NULL, log);
        panic(result_error("Fatal OpenGL Error", format("Failed to compile shader '%s.%s'!\n%s", name, type == GL_VERTEX_SHADER ? "vert" : "frag", log)));
    }

    free(path);
    free(source);
    return result_no_error();
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
        panic(result_error("Fatal OpenGL Error", "Failed to link shader '%s'!\n%s", name, log));
    }

    // VAO & VBO
    glGenVertexArrays(1, &out->vao);
    glGenBuffers(1, &out->vbo);

    // Uniform table
    out->uniform_table = hashtable_string();

    return result_no_error();
}

void shader_bind(shader_t *this) {
    glUseProgram(this->program);
}

void shader_uniform_mat4(shader_t *this, GLuint location, mat4 data) {
    shader_bind(this);
    glUniformMatrix4fv(location, 1, false, &data[0][0]);
}
