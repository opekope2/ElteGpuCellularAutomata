#pragma once

#include "cellular_automaton.hpp"
#include <CL/cl_platform.h>
#include <cstddef>
#include <format>
#include <vector>

#define ANT_MASK 0x80
#define VALUE_MASK 0x7F
#define FACING_MASK 0x0F

#define MAX_VALUE 0x80
#define MAX_STATE 0x100

#define FACING_NORTH 0x01
#define FACING_EAST 0x02
#define FACING_SOUTH 0x04
#define FACING_WEST 0x08

#define TURN_NONE 0x01
#define TURN_RIGHT 0x02
#define TURN_REVERSE 0x04
#define TURN_LEFT 0x08

typedef cl_uchar turn_t;
typedef cl_uchar state_t;

typedef struct TurmiteRule {
    cell_t newValue;
    turn_t turn;
    state_t nextState;
} turmite_rule_t;

typedef struct Ant {
    uint xy;
    cl_uchar facing;
} ant_t;

struct TurmiteRuleset {
    std::vector<turmite_rule_t> rules;
    cell_t nCellValues;
    state_t nStates;
};

template <>
struct std::formatter<turmite_rule_t> {
    constexpr auto parse(format_parse_context &ctx) { return ctx.begin(); }
    auto format(const turmite_rule_t &rule, format_context &ctx) const {
        return format_to(ctx.out(), "{{{},{},{}}}", rule.newValue, rule.turn, rule.nextState);
    }
};

template <>
struct std::formatter<TurmiteRuleset> {
    constexpr auto parse(format_parse_context &ctx) { return ctx.begin(); }
    auto format(const TurmiteRuleset &ruleset, format_context &ctx) const {
        auto out = format_to(ctx.out(), "{{");

        for (size_t i = 0; i < ruleset.nStates; i++) {
            out = i ? format_to(out, ",{{")
                    : format_to(out, "{{");
            for (size_t j = 0; j < ruleset.nCellValues; j++) {
                turmite_rule_t rule = ruleset.rules[i * ruleset.nCellValues + j];
                out = j ? format_to(out, ",{}", rule)
                        : format_to(out, "{}", rule);
            }
            out = format_to(out, "}}");
        }

        return format_to(out, "}}");
    }
};
