#version 330

in vec2 uv;
in vec2 pos;

uniform sampler2D tex;

out vec4 color;

void main() {

    float cloud = texture(tex, uv).r;

    color = vec4(vec3(1.0), cloud);

}
