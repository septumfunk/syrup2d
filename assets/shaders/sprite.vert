#version 430 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texcoord;
out vec2 f_texcoord;
layout (location = 2) uniform mat4 ortho;

void main() {
    gl_Position = ortho * vec4(position, 1.0);
    f_texcoord = texcoord;
}