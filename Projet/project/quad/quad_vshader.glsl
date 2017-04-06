#version 330

in vec2 position;

out vec3 light_dir, view_dir;
out vec4 vpoint_mv;

uniform mat4 MVP;
uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform sampler2D tex;
uniform vec3 light_pos;

void main() {
    mat4 MV = view * model;
    vec2 uv = (position + vec2(1.0, 1.0)) * 0.5;
    //gl_Position = MVP * vec4(position[0], position[1], texture(tex, uv).r, 1.0);
    vpoint_mv = MV * vec4(position[0],position[1],texture(tex,uv).r, 1.0);
    gl_Position = projection * vpoint_mv;
    light_dir = normalize(light_pos-vec3(vpoint_mv));
    view_dir = normalize(-vec3(vpoint_mv));
}
