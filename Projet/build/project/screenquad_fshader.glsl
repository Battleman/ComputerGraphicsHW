#version 330

in vec2 uv;


out vec3 color;

uniform sampler2D tex1;
uniform float tex_width;
uniform float tex_height;

void main() {
    color = texture(tex1,uv).rgb;
}

