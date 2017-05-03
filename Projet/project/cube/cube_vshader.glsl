#version 330 core
uniform mat4 MVP;
in vec3 vpoint;
in vec2 vtexcoord;
out vec2 uv;

void main(){
//    gl_Position =  MVP * R(50*time) * T(1,0,0) * R(50*time)* vec4(vpoint,1); ///< spin-circ
//    gl_Position =  MVP * R(50*time)* vec4(vpoint,1); ///< spin
    gl_Position =  MVP * vec4(vpoint,1); ///< still
    uv = vtexcoord;
}
