#version 330

in vec3 material;
in vec4 vpoint_mv;
in vec3 light_dir, view_dir;
in float height;

uniform vec3 La, Ld, Ls;
uniform float alpha;

out vec3 color;

void main() {
    vec3 X = vec3(dFdx(vpoint_mv));
    vec3 Y = vec3(dFdy(vpoint_mv));
    vec3 normal_mv = normalize(cross(X,Y));

    vec3 res_color = vec3(0.0f);
    //I'm keeping ambient and specular in case we need them for later
    //vec3 r = normalize((2.0f*normal_mv*dot(normal_mv,light_dir))-light_dir);
    //res_color += (material*La);
    res_color += (material*max(0.0f,dot(normal_mv,light_dir))*Ld);
    //res_color += (material*pow(max(0.0f,dot(r,view_dir)),alpha)*Ls);
    color = res_color;


}
