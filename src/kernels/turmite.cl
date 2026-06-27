#define WRAP_AROUND(wrap, size) ((wrap) ? (size) : 0)

#define ANT_MASK 0x80
#define VALUE_MASK 0x7F
#define FACING_MASK 0x0F

#define FACING_NORTH 0x01
#define FACING_EAST 0x02
#define FACING_SOUTH 0x04
#define FACING_WEST 0x08

#define TURN_NONE 0x01
#define TURN_RIGHT 0x02
#define TURN_REVERSE 0x04
#define TURN_LEFT 0x08

typedef uchar turn_t;
typedef uchar state_t;

typedef struct TurmiteRule {
    cell_t newValue;
    turn_t turn;
    state_t nextState;
} turmite_rule_t;

typedef struct Ant {
    uint xy;
    uchar facing;
} ant_t;

uchar turn(uchar facing, uchar amount) { return ((facing << ctz(amount)) | (facing >> (4 - ctz(amount)))) & FACING_MASK; }

uint forward(uint w, uint h, uint id, uchar facing) {
    uint size = w * h;

    switch (facing) {
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

kernel void turmiteStep(uint width,
                        uint height,
                        global ant_t *ant,
                        global state_t *state,
                        cell_t nCellValues,
                        const global turmite_rule_t *rules,
                        global cell_t *cells) {
    uint id = ant->xy;
    cell_t value = cells[id] & VALUE_MASK;
    turmite_rule_t rule = rules[*state * nCellValues + value];

    cells[id] = rule.newValue;

    ant->facing = turn(ant->facing, rule.turn);
    uint next = forward(width, height, id, ant->facing);

    cells[next] |= ANT_MASK;
    ant->xy = next;

    *state = rule.nextState;
}
