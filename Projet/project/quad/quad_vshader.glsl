#version 330

in vec2 position;
in vec2 vtexcoord;

out vec2 uv;

uniform mat4 MVP;
uniform sampler2D tex;

void main() {

    uv = (position + vec2(1.0, 1.0)) * 0.5;
    gl_Position = MVP * vec4(position[0], position[1], texture(tex, uv).r, 1.0);
}
