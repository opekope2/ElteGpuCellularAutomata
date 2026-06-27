#version 330 core

layout(origin_upper_left, pixel_center_integer) in vec4 gl_FragCoord;
out vec4 FragColor;
uniform usampler2D data;
uniform ivec2 res;
uniform ivec2 offset;
uniform int zoom;

const uint ANT_MASK = 0x80u;
const uint VALUE_MASK = 0x7Fu;

const vec4 BLACK = vec4(0, 0, 0, 1);
const vec4 RED = vec4(1, 0, 0, 1);
const vec4 GREEN = vec4(0, 1, 0, 1);
const vec4 BLUE = vec4(0, 0, 1, 1);
const vec4 CYAN = vec4(0, 1, 1, 1);
const vec4 MAGENTA = vec4(1, 0, 1, 0);
const vec4 YELLOW = vec4(1, 1, 0, 1);
const vec4 WHITE = vec4(1, 1, 1, 1);

// https://stackoverflow.com/a/17897228
vec3 hsv2rgb(vec3 c) {
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

// Non-linear hue
uint reverse5(uint value) {
    uint a = value & 0x01u;
    uint b = value & 0x02u;
    uint c = value & 0x04u;
    uint d = value & 0x08u;
    uint e = value & 0x10u;

    return (a << 4) | (b << 2) | c | (d >> 2) | (e >> 4);
}

vec3 palette(uint value) {
    // Stay away from red as it's the ant color
    float hue = float(reverse5(value & 0x1Fu) + 2u) / 36.0;
    float saturation = 1.0 - 0.2 * (value >> 5);

    return hsv2rgb(vec3(hue, saturation, 1.0));
}

void main() {
    ivec2 size = textureSize(data, 0);
    vec2 halfRes = vec2(res) / 2;
    vec2 coord = gl_FragCoord.xy - halfRes;
    vec2 zoomedCoord = vec2(offset) + coord / zoom;
    ivec2 cellCoord = (ivec2(floor(zoomedCoord)) % size + size) % size;
    uint cell = texelFetch(data, cellCoord, 0).x;

    if ((cell & ANT_MASK) == ANT_MASK)
        FragColor = RED;
    else if ((cell & VALUE_MASK) == 0u)
        FragColor = WHITE;
    else if ((cell & VALUE_MASK) == 1u)
        FragColor = BLACK;
    else
        FragColor = vec4(palette(cell & VALUE_MASK), 1.0);
}
