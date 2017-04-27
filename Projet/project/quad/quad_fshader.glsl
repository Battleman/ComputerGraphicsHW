#version 330

in vec2 uv;
in vec3 material;
in vec4 vpoint_mv;
in vec3 light_dir, view_dir;
in float height;

uniform float isWater;
uniform vec3 La, Ld, Ls;
uniform float alpha;
uniform sampler2D tex;
uniform int discard_pix;

out vec4 color;

void main() {
    if(isWater == 0) {
        vec3 X = vec3(dFdx(vpoint_mv));
        vec3 Y = vec3(dFdy(vpoint_mv));
        vec3 normal_mv = normalize(cross(X,Y));

        vec3 res_color = vec3(0.0f);
        //I'm keeping ambient and specular in case we need them for later
        //vec3 r = normalize((2.0f*normal_mv*dot(normal_mv,light_dir))-light_dir);
        //res_color += (material*La);
        res_color += (material*max(0.0f,dot(normal_mv,light_dir))*Ld);
        //res_color += (material*pow(max(0.0f,dot(r,view_dir)),alpha)*Ls);
        if(discard_pix != 0 && height < 0) {
            discard;
        }
        color = vec4(res_color,1.0f);
    } else {
        int window_width = textureSize(tex, 0).x;
        int window_height = textureSize(tex, 0).y;

        float _u = gl_FragCoord.x/window_width;
        float _v = gl_FragCoord.y/window_height;

        vec3 res_color = vec3(0.0);
        vec3 color_from_texture = vec3(0.5, 0.5, 0.85);
        vec3 color_from_mirror = texture(tex,vec2(_u, _v)).rgb;
        res_color = mix(color_from_texture, color_from_mirror, vec3(.35));
        color = vec4(res_color.x,res_color.y,res_color.z,0.7);
//        color = color_from_texture;
    }


}
