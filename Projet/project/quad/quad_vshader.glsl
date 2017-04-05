#version 330

in vec2 position;
in vec2 vtexcoord;

out vec2 uv;

uniform mat4 MVP;

void main() {

    gl_Position = MVP * vec4(position[0],position[1],0.0,1.0);
    uv = (position + vec2(1.0, 1.0)) * 0.5;
}
