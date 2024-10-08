#version 330 core
out vec4 o_fragcolor;
in vec2 i_texcoord;
uniform sampler2D u_texture;

void main() {
    o_fragcolor = texture(u_texture, i_texcoord);
}