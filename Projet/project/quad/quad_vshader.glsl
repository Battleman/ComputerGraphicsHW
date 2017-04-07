#version 330

in vec2 position;

out vec3 light_dir, view_dir;
out vec4 vpoint_mv;
out vec3 material;

uniform mat4 MVP;
uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform sampler2D tex;
uniform vec3 light_pos;

void main() {
    mat4 MV = view * model;
    vec2 uv = (position + vec2(1.0, 1.0)) * 0.5;
    float height = texture(tex,uv).r;
    //gl_Position = MVP * vec4(position[0], position[1], texture(tex, uv).r, 1.0);
    vpoint_mv = MV * vec4(position[0],position[1],height, 1.0);
    gl_Position = projection * vpoint_mv;
    light_dir = normalize(light_pos-vec3(vpoint_mv));
    view_dir = normalize(-vec3(vpoint_mv));
    //Here I ll be using a gaussian function to make the color change smoother
    vec3 height_material = vec3(0.0,0.0,0.0);
    float variance = 0.0;
    float color_height = 0.0;
    //ground: brown, at height -0.35, spread: 0.1
    variance = 0.1;
    color_height = -0.35;
    height_material += exp((-pow((height-color_height),2))/(2*variance*variance)) * vec3(0.3,0.2,0.0);
    //sand: yellow, at height -0.25, spread: 0.05
    variance = 0.05;
    color_height = -0.25;
    height_material += exp((-pow((height-color_height),2))/(2*variance*variance)) * vec3(0.8,0.8,0.0);
    //grass: green, at height -0.15, spread: 0.075
    variance = 0.075;
    color_height = -0.15;
    height_material += exp((-pow((height-color_height),2))/(2*variance*variance)) * vec3(0.2,0.8,0.0);
    //mountain: grey, at height 0.2, spread: 0.3
    variance = 0.3;
    color_height = 0.2;
    height_material += exp((-pow((height-color_height),2))/(2*variance*variance)) * vec3(0.5,0.5,0.5);
    //mountain top: white, at height 1.0, spread: 0.7
    variance = 0.1;
    color_height = 0.5;
    height_material += exp((-pow((height-color_height),2))/(2*variance*variance)) * vec3(1.0,1.0,1.0);
    if(height < -0.45) {
        height_material = vec3(0.0,0.0,1.0); // water
    }

    material = height_material;


}
