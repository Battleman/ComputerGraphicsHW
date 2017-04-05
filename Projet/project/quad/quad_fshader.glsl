#version 330

in vec2 uv;

out vec3 color;

uniform sampler2D tex;

void main() {
    float value = texture(tex,uv).r;
    color = vec3(value, value, value);
}
