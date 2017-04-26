#version 330

in vec2 position;

out vec3 light_dir, view_dir;
out vec4 vpoint_mv;
out vec3 material;
out float height;
//out vec3 normal_mv;

uniform float isWater;
uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform sampler2D tex;
uniform sampler2D tex2;
uniform sampler2D tex3;
uniform sampler2D tex4;
uniform vec3 light_pos;
uniform int triangles_number;

void main() {
    mat4 MV = view * model;
    vec2 uv = (position + vec2(1.0, 1.0)) * 0.5;

    if(isWater == 0) {
        height = texture(tex,uv).r;

        vpoint_mv = MV * vec4(position.x,position.y,height, 1.0);
        gl_Position = projection * vpoint_mv;
        light_dir = normalize(light_pos-vec3(vpoint_mv));
        view_dir = normalize(-vec3(vpoint_mv));

        /* I'm keeping this piece of code in case I'd need to compute normals in the vertex shader
        float offset = 1.0f/float(triangles_number*1.0);
        vec4 X0point_mv = MV * vec4(position.x-offset,position.y,texture(tex,vec2(((position.x-offset)/2.0)+0.5,uv.y)).r, 1.0);
        vec4 Y0point_mv = MV * vec4(position.x,position.y-offset,texture(tex,vec2(uv.x,((position.y-offset)/2.0)+0.5)).r, 1.0);
        vec4 X1point_mv = MV * vec4(position.x+offset,position.y,texture(tex,vec2(((position.x+offset)/2.0)+0.5,uv.y)).r, 1.0);
        vec4 Y1point_mv = MV * vec4(position.x,position.y+offset,texture(tex,vec2(uv.x,((position.y+offset)/2.0)+0.5)).r, 1.0);

        vec3 X = X1point_mv.xyz - X0point_mv.xyz;
        vec3 Y = Y1point_mv.xyz - Y0point_mv.xyz;
        normal_mv = normalize(cross(X,Y));
        */
    } else {
        vpoint_mv = MV * vec4(position.x,position.y,-0.45, 1.0);
        gl_Position = projection * vpoint_mv;
        light_dir = normalize(light_pos-vec3(vpoint_mv));
        view_dir = normalize(-vec3(vpoint_mv));
    }

    //Next I'll be using a gaussian function to make the color changes smoother
    vec3 height_material = vec3(0.0,0.0,0.0);
    float variance = 0.0;
    float color_height = 0.0;

    //sand: yellow, at height -0.45, spread: -0.45 to -0.35
    variance = 0.1;
    color_height = -0.45;
    height_material += exp((-pow((height-color_height),2))/(2*variance*variance)) * texture(tex4,uv).rgb;
    //dirt: brown, at height -0.30, spread: -0.35 to -0.25
    variance = 0.05;
    color_height = -0.30;
    height_material += exp((-pow((height-color_height),2))/(2*variance*variance)) * vec3(0.3,0.2,0.0);
    //grass/trees: green, at height -0.15, spread: -0.25 to -0.05
    variance = 0.1;
    color_height = -0.15;
    height_material += exp((-pow((height-color_height),2))/(2*variance*variance)) * texture(tex3,uv).rgb;
    //mountain: grey, at height 0.15, spread: -0.05 to 0.35
    variance = 0.20;
    color_height = 0.15;
    height_material += exp((-pow((height-color_height),2))/(2*variance*variance)) * texture(tex2,uv).rgb;
    //mountain top: white, at height 0.40, spread: 0.35 to 0.40
    variance = 0.05;
    color_height = 0.40;
    height_material += exp((-pow((height-color_height),2))/(2*variance*variance)) * vec3(1.0,1.0,1.0);
    if(height < -0.45) {
        height_material = vec3(0.0,0.0,0.7); // water
    }
    if(height > 0.40) {
        height_material = exp((-pow((height-color_height),2))/(2*0.4*0.4)) * vec3(1.0,1.0,1.0); // snow
    }

    material = height_material;

}
