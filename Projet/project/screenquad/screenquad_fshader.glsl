#version 330

in vec2 uv;


out vec3 color;

uniform float tex_width;
uniform float tex_height;
//uniform float permutation[256];
uniform int[512] p;
//int p[256] = int[256](151,160,137,91,90,15,
//        131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
//        190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
//        88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
//        77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
//        102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
//        135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
//        5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
//        223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
//        129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
//        251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
//        49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
//        138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
//        );


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

    return (noise+1)/2;
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
    float color1 = fBm(uv.x, uv.y, 0.9, 3, 7);
    color = vec3(color1);

}

