#define WRAP_AROUND(wrap, size) ((wrap) ? (size) : 0)

#define WHITE 0x01
#define ANT 0x02
#define FACING (0x04 | 0x08)
#define FACING_OFFSET 0x04

#define FACING_NORTH (0 * FACING_OFFSET)
#define FACING_EAST (1 * FACING_OFFSET)
#define FACING_SOUTH (2 * FACING_OFFSET)
#define FACING_WEST (3 * FACING_OFFSET)

cell_t cw(cell_t facing) { return (facing + FACING_OFFSET) & FACING; }
cell_t ccw(cell_t facing) { return (facing - FACING_OFFSET) & FACING; }

uint forward(uint w, uint h, uint id, cell_t facing) {
    uint size = w * h;

    switch (facing & FACING) {
    case FACING_NORTH:
        return id - w + WRAP_AROUND(id < w, size);
    case FACING_EAST:
        return id + 1 - WRAP_AROUND(id % w == w - 1, w);
    case FACING_SOUTH:
        return id + w - WRAP_AROUND(id >= w * (h - 1), size);
    case FACING_WEST:
        return id - 1 + WRAP_AROUND(id % w == 0, w);
    default:
        return id;
    }
}

kernel void langtonStep(uint width, uint height, global uint *antXY, global cell_t *cells) {
    uint id = *antXY;
    cell_t c = cells[id];

    bool white = (c & WHITE) == WHITE;
    cell_t facing = white ? cw(c & FACING) : ccw(c & FACING);
    uint next = forward(width, height, id, facing);

    cells[id] = white ? 0 : WHITE;
    cells[next] = (cells[next] & WHITE) | ANT | facing;

    *antXY = next;
}
