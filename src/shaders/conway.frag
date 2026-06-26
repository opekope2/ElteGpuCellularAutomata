#version 330 core

layout(origin_upper_left, pixel_center_integer) in vec4 gl_FragCoord;
out vec4 FragColor;
uniform usampler2D data;
uniform ivec2 res;
uniform ivec2 offset;
uniform int zoom;

const uint DEAD = 0u;
const uint ALIVE = 1u;

const vec4 BLACK = vec4(0, 0, 0, 1);
const vec4 RED = vec4(1, 0, 0, 1);
const vec4 GREEN = vec4(0, 1, 0, 1);
const vec4 BLUE = vec4(0, 0, 1, 1);
const vec4 CYAN = vec4(0, 1, 1, 1);
const vec4 MAGENTA = vec4(1, 0, 1, 0);
const vec4 YELLOW = vec4(1, 1, 0, 1);
const vec4 WHITE = vec4(1, 1, 1, 1);

void main() {
    ivec2 size = textureSize(data, 0);
    vec2 halfRes = vec2(res) / 2;
    vec2 coord = gl_FragCoord.xy - halfRes;
    vec2 zoomedCoord = vec2(offset) + coord / zoom;
    ivec2 cellCoord = (ivec2(floor(zoomedCoord)) % size + size) % size;
    uint cell = texelFetch(data, cellCoord, 0).x;
    FragColor = cell == ALIVE ? YELLOW : BLACK;
}
