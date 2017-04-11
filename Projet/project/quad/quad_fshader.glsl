#version 330

in vec3 material;
in vec3 normal_mv;
in vec4 vpoint_mv;
in vec3 light_dir, view_dir;
in float height;

uniform vec3 La, Ld, Ls;
uniform vec3 ka, kd, ks;
uniform float alpha;
uniform sampler2D tex;

out vec3 color;

void main() {
    //float value = texture(tex,uv).r;
    //color = vec3(value, value, value);

//    vec3 X = vec3(dFdx(vpoint_mv));
//    vec3 Y = vec3(dFdy(vpoint_mv));
//    vec3 normal_mv = normalize(cross(X,Y));


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

    vec3 material = height_material;
    //material = vec3(0.0,1.0,0.0);

    vec3 res_color = vec3(0.0f);
    //I'm keeping ambient and specular in case we need them for later
    //vec3 r = normalize((2.0f*normal_mv*dot(normal_mv,light_dir))-light_dir);
    //res_color += (material*La);
    res_color += (material*max(0.0f,dot(normal_mv,light_dir))*Ld);
    //res_color += (material*pow(max(0.0f,dot(r,view_dir)),alpha)*Ls);
    color = res_color;


}
