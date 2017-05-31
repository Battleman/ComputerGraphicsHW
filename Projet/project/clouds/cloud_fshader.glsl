#version 330

in vec2 uv;
in vec2 pos;
in float cloud;

uniform sampler2D tex;
uniform float speed;

out vec4 color;

void main() {

    color = vec4(vec3(1.0), cloud);

}
