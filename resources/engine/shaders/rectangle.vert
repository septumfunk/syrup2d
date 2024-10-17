#version 430 core
%s
layout (location = 0) in vec2 position;
layout (location = 1) in vec4 color;
out vec4 f_color;

void main() {
    gl_Position = projection * camera * vec4(position, 0.0, 1.0);
    f_color = color;
}