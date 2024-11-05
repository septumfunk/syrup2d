#version 430 core
out vec4 fragcolor;
in vec2 f_texcoord;
uniform sampler2D sampler;

void main() {
    fragcolor = texture(sampler, f_texcoord);
}