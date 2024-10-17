#version 430 core
%s
out vec4 fragcolor;
in vec2 f_texcoord;
in vec4 f_tint;
uniform sampler2D sampler;

void main() {
    vec4 tex_color = texture2D(sampler, f_texcoord);
    fragcolor = tex_color * f_tint;
    if (fragcolor.a == 0.0)
        discard;
}