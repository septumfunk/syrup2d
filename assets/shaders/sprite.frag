#version 430 core
%s
out vec4 fragcolor;
in vec2 f_texcoord;
uniform sampler2D sampler;

void main() {
    fragcolor = texture(sampler, f_texcoord);
    if (fragcolor.a == 0.0)
        discard;
}