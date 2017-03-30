#version 330

in vec2 uv;
uniform int SIZE;
uniform float G[31];    //THIS VALUE MUST BE EQUAL TO SIZE!!!
uniform int horizontal;

out vec3 color;

uniform sampler2D tex1;
uniform float tex_width;
uniform float tex_height;

void main() {
    vec3 color_tot = vec3(0,0,0);
    float weight_tot = 0;
    if(horizontal == 1){
        for(int i=0; i<SIZE; i++){
            vec3 neigh_color = texture(tex1, uv+vec2((i-(SIZE/2))/tex_width,0)).rgb;
            color_tot += G[i] * neigh_color;
            weight_tot += G[i];
        }
    } else {
        for(int i=0; i<SIZE; i++){
            vec3 neigh_color = texture(tex1, uv+vec2(0,(i-(SIZE/2))/tex_height)).rgb;
            color_tot += G[i] * neigh_color;
            weight_tot += G[i];
        }
    }
    color = color_tot / weight_tot; // ensure \int w = 1
}

