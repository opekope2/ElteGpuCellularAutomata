#define DEAD 0
#define ALIVE 1
#define WRAP_AROUND(wrap, size) ((wrap) ? (size) : 0)

uint8 getNeighbors(uint w, uint h, uint id) {
    uint size = w * h;

    uint _n = id - w + WRAP_AROUND(id < w, size);
    uint _e = id + 1 - WRAP_AROUND(id % w == w - 1, w);
    uint _s = id + w - WRAP_AROUND(id >= w * (h - 1), size);
    uint _w = id - 1 + WRAP_AROUND(id % w == 0, w);

    uint _ne = _n + 1 - WRAP_AROUND(_n % w == w - 1, w);
    uint _se = _s + 1 - WRAP_AROUND(_s % w == w - 1, w);
    uint _sw = _s - 1 + WRAP_AROUND(_s % w == 0, w);
    uint _nw = _n - 1 + WRAP_AROUND(_n % w == 0, w);

    return (uint8)(_n, _ne, _e, _se, _s, _sw, _w, _nw);
}

// TODO parallel
// TODO check data boundaries
kernel void loadRle(uint width, uint x, uint y, const global char *data, global cell_t *simulation) {
    ulong length = 0;
    uint offset = x + y * width, pos = offset;

    for (char c = *data; c != '!'; c = *++data)
        if (c >= '0' && c <= '9')
            length = (10 * length) + (c - '0');
        else if (c == '$')
            pos = offset = offset + max(length, 1ul) * width, length = 0;
        else if (c == 'b' || c == 'o')
            for (length = max(length, 1ul); length > 0; length--)
                simulation[pos++] = c == 'o';
}

kernel void conwayStep(rule_t rule, const global cell_t *old, global cell_t *current) {
    uint x = get_global_id(0);
    uint y = get_global_id(1);
    uint w = get_global_size(0);
    uint h = get_global_size(1);
    uint id = x + y * w;
    cell_t c = old[id];

    uint8 neighbors = getNeighbors(w, h, id);
    ushort n = 1;
    for (uint i = 0; i < 8; i++)
        n <<= old[neighbors[i]];

    current[id] = (c == DEAD && (n & rule.birth)) || (c == ALIVE && (n & rule.survive)) ? ALIVE : DEAD;
}
