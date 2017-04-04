#version 330

in vec2 position;
in vec2 vtexcoord;

out vec2 uv;

uniform mat4 MVP;
uniform float permutation[256];

float mix(float x, float y , float f) {
  return (1 - f)*x + f*y;
}

float fun(float t) {
  return t*t*t*(t*(t*6.0 -15.0) + 10.0);
}

void main() {

    gl_Position = MVP * vec4(position[0],position[1],0.0,1.0);
    uv = (position + vec2(1.0, 1.0)) * 0.5;
}
