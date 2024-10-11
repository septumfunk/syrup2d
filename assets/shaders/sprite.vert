#version 430 core
%s
layout (location = 0) in vec2 position;
layout (location = 1) in vec2 texcoord;
out vec2 f_texcoord;

void main() {
    gl_Position = projection * camera * vec4(position, 0.0, 1.0);
    f_texcoord = texcoord;
}