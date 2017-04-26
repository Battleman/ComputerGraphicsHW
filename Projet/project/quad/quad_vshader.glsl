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

}
