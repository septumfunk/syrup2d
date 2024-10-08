#version 430 core
layout (location = 0) in vec3 i_position;
layout (location = 1) in vec2 i_texcoord;
out vec2 o_texcoord;
layout (location = 2) uniform mat4 ortho;

void main() {
    gl_Position = ortho * vec4(i_position, 1.0);
    o_texcoord = i_texcoord;
}