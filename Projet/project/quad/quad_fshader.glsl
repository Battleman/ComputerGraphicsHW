#version 330

in vec2 uv;
in vec3 material;
in vec4 vpoint_mv;
in vec3 light_dir, view_dir;
in float height;
in float visibility;


uniform vec3 La, Ld, Ls;
uniform float alpha;
uniform sampler2D tex;
uniform int discard_pix;

out vec4 color;

void main() {
    /*We have 2 quads : the terrain and the water. This function treats both cases*/
    vec3 light_dirn = normalize(light_dir);
    vec3 view_dirn = normalize(view_dir);
    vec3 X = vec3(dFdx(vpoint_mv));
    vec3 Y = vec3(dFdy(vpoint_mv));
    vec3 normal_mv = normalize(cross(X,Y));

    vec3 res_color = vec3(0.0f);
    res_color += (material*max(0.0f,dot(normal_mv,light_dirn))*Ld);
    if(discard_pix != 0 && height < 0) {
        discard;
    }
    color = vec4(res_color,visibility);

}
