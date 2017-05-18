#version 330

in vec2 position;

out vec3 light_dir, view_dir;
out vec4 vpoint_mv;
out vec3 water;
out vec2 dudv;
out float visibility;
//out vec3 normal_mv;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform sampler2D tex;
uniform sampler2D normalMap;
uniform sampler2D dudvMap;
uniform vec3 light_pos;
uniform int triangles_number;

uniform float time;
uniform float speed;

const float density = 0.2;
const float gradient = 1.5;

const float wave_strength = 0.007;
const float tiling = 4.0;

void main() {
    mat4 MV = view * model;
    vec2 uv = (position + vec2(10.0, 10.0)) * 0.05;
    vec3 normal_mv = vec3(0.0);

    float height = 0.001*sin((position.x+position.y-time)*6.0f);

    vpoint_mv = MV * vec4(position.x,position.y,height, 1.0);
    gl_Position = projection * vpoint_mv;

    float distance = length(vpoint_mv.xyz);
    visibility = exp(-pow((distance*density),gradient));
    visibility = clamp(visibility, 0.0, 1.0);

    light_dir = normalize(light_pos-vec3(vpoint_mv));
    view_dir = normalize(-vec3(vpoint_mv));

    water = texture(normalMap, vec2(uv.x + speed, uv.y)*tiling).rgb;
    dudv = (texture(dudvMap, vec2(uv.x + speed, uv.y)*tiling).rg * 2.0 - 1.0) * wave_strength;
    dudv += (texture(dudvMap, vec2(- uv.x - speed, uv.y + speed)*tiling).rg * 2.0 - 1.0) * wave_strength;

}
