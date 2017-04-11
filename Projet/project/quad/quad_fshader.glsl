#version 330

in vec3 material;
in vec4 vpoint_mv;
in vec3 light_dir, view_dir;
in float height;

uniform vec3 La, Ld, Ls;
uniform float alpha;
uniform sampler2D tex;

out vec3 color;

void main() {
    vec3 X = vec3(dFdx(vpoint_mv));
    vec3 Y = vec3(dFdy(vpoint_mv));
    vec3 normal_mv = normalize(cross(X,Y));

    //Here I ll be using a gaussian function to make the color change smoother
    vec3 height_material = vec3(0.0,0.0,0.0);
    float variance = 0.0;
    float color_height = 0.0;

    //sand: yellow, at height -0.40, spread: -0.45 to -0.40
    variance = 0.05;
    color_height = -0.45;
    height_material += exp((-pow((height-color_height),2))/(2*variance*variance)) * vec3(0.65,0.65,0.0);
    //dirt: brown, at height -0.20, spread: -0.40 to -0.30
    variance = 0.05;
    color_height = -0.35;
    height_material += exp((-pow((height-color_height),2))/(2*variance*variance)) * vec3(0.3,0.2,0.0);
    //grass/trees: green, at height -0.25, spread: -0.30 to -0.20
    variance = 0.05;
    color_height = -0.25;
    height_material += exp((-pow((height-color_height),2))/(2*variance*variance)) * vec3(0.0,0.3,0.0);
    //mountain: grey, at height 0.0, spread: -0.20 to 0.20
    variance = 0.2;
    color_height = 0.0;
    height_material += exp((-pow((height-color_height),2))/(2*variance*variance)) * vec3(0.5,0.5,0.5);
    //mountain top: white, at height 0.25, spread: 0.20 to 0.25
    variance = 0.05;
    color_height = 0.25;
    height_material += exp((-pow((height-color_height),2))/(2*variance*variance)) * vec3(1.0,1.0,1.0);
    if(height < -0.45) {
        height_material = vec3(0.0,0.0,0.7); // water
    }
    if(height > 0.25) {
        height_material = vec3(1.0,1.0,1.0); // snow
    }

    vec3 material = height_material;

    vec3 res_color = vec3(0.0f);
    //I'm keeping ambient and specular in case we need them for later
    //vec3 r = normalize((2.0f*normal_mv*dot(normal_mv,light_dir))-light_dir);
    //res_color += (material*La);
    res_color += (material*max(0.0f,dot(normal_mv,light_dir))*Ld);
    //res_color += (material*pow(max(0.0f,dot(r,view_dir)),alpha)*Ls);
    color = res_color;


}
