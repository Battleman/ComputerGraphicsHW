#version 330

in vec3 material;
in vec4 vpoint_mv;
in vec3 light_dir, view_dir;

uniform vec3 La, Ld, Ls;
uniform vec3 ka, kd, ks;
uniform float alpha;
uniform sampler2D tex;

out vec3 color;




void main() {
    //float value = texture(tex,uv).r;
    //color = vec3(value, value, value);
//    vec4 v = normalize(vpoint_mv);
//    vec3 l = normalize(light_dir);
    vec3 X = vec3(dFdx(vpoint_mv));
    vec3 Y = vec3(dFdy(vpoint_mv));
    vec3 normal_mv = normalize(cross(X,Y));
    vec3 r = normalize((2.0f*normal_mv*dot(normal_mv,light_dir))-light_dir);
    vec3 res_color = vec3(0.0f);
    //I'm keeping ambient and specular in case we need them for later
    //res_color += (ka*La);
    res_color += (material*max(0.0f,dot(normal_mv,light_dir))*Ld);
    //res_color += (ks*pow(max(0.0f,dot(r,view_dir)),alpha)*Ls);
    color = res_color;
}
