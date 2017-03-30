#version 330 core
out vec3 color;
in vec2 uv;
in vec4 gl_FragCoord;
uniform sampler2D tex;
uniform sampler2D tex_mirror;

void main() {

    int window_width = textureSize(tex_mirror, 0).x;
    int window_height = textureSize(tex_mirror, 0).y;

    float _u = gl_FragCoord.x/window_width;
    float _v = gl_FragCoord.y/window_height;

    vec3 color_from_texture = texture(tex,uv).rgb;
    vec3 color_from_mirror = texture(tex_mirror,vec2(_u, 1-_v)).rgb;
    color = mix(color_from_texture, color_from_mirror, vec3(.15)); // blend texture with mirror image
}
