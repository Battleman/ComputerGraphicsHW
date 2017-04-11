#version 330

in vec2 uv;


out vec3 color;

uniform float tex_width;
uniform float tex_height;
//uniform float permutation[256];
uniform int[512] p;

int repeat = 256;

int inc(int num) {
    num++;
    if(repeat > 0) num = int(mod(num, repeat));
    return num;
}

float mix(float x, float y , float f) {
  return (1 - f)*x + f*y;
}

float fade(float t) {
  return t*t*t*(t*(t*6.0 - 15.0) + 10.0);
}

float grad(int hash, float x, float y) {
    int h = hash & 3;

    if(h == 0x0) {
        return x + y;
    } else if(h == 0x1) {
        return -x + y;
    } else if(h == 0x2) {
        return x - y;
    } else if(h == 0x3){
        return -x - y;
    }
    return 0.0f;
}

float Perlin(float x, float y) {
    if(repeat > 0) {
        x = mod(x, repeat);
        y = mod(y, repeat);
    }

    int xi = int(x) & 255;
    int yi = int(y) & 255;
    float xf = x - int(x);
    float yf = y - int(y);

    float u = fade(xf);
    float v = fade(yf);

    int aa, ab, ba, bb;
    aa = p[p[    xi ]+    yi ];
    ab = p[p[    xi ]+inc(yi)];
    ba = p[p[inc(xi)]+    yi ];
    bb = p[p[inc(xi)]+inc(yi)];

    float st, uv, noise;
    st = mix(grad(aa, xf  , yf),
             grad(ba, xf-1, yf),
             u);
    uv = mix(grad(ab, xf  , yf-1),
             grad(bb, xf-1, yf-1),
             u);
    noise = mix(st, uv, v);

    return noise;
}

float fBm(float x, float y, float H, float lacunarity, int octaves) {
    float value = 0.0;
    /* inner loop of fractal construction */
    for (int i = 0; i < octaves; i++) {
        value += Perlin(x, y) * pow(lacunarity, -H*i);
        x *= lacunarity;
        y *= lacunarity;
    }
    return value;
}

float OctavePerlin(float x, float y, int octaves, float persistence) {
    float total = 0;
    float frequency = 1;
    float amplitude = 1;
    float maxValue = 0;  // Used for normalizing result to 0.0 - 1.0
    for(int i=0;i<octaves;i++) {
        total += Perlin(x * frequency, y * frequency) * amplitude;

        maxValue += amplitude;

        amplitude *= persistence;
        frequency *= 2;
    }



    return total/maxValue;
}

void main() {
//    float color1 = OctavePerlin(4*uv.x, 4*uv.y, 10, 0.3);
    float color1 = fBm(uv.x, uv.y, 0.85, 3, 4);
//    float color1 = Perlin(uv.x,uv.y);
    color = vec3(color1);

}

