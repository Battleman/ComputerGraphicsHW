#version 330 core
in vec2 uv;
uniform sampler2D colorTex;
uniform sampler2D velocityTex;
out vec4 color;

void main() {
    /// TODO: use the velocity vector stored in velocityTex to compute the line integral
    /// TODO: use a constant number of samples for integral (what happens if you take too few?)
    vec2 velocity = texture(velocityTex, uv).xy;
//    vec2 velocity = vec2(1,0);
    vec4 color_tot = vec4(0,0,0,0);
    float total = 30.0f;
    for(int i = 0; i < total; i++) {
        vec4 neigh_color = texture(colorTex, uv + vec2(velocity.x*i/total, velocity.y*i/total));
        color_tot += neigh_color;
    }
    color = color_tot/total;
    /// HINT: you can scale the velocity vector to make the motion blur effect more prominent
    /// HINT: to debug integration don't use the velocityTex, simply assume velocity is constant, e.g. vec2(1,0)
}
