#pragma once

#include <CL/cl_platform.h>
#include <format>

using namespace std;

typedef cl_uchar cell_t;

typedef struct Rule {
    cl_ushort birth = 1 << 3;
    cl_ushort survive = 1 << 3 | 1 << 2;
} rule_t;

template <>
struct std::formatter<Rule> {
    constexpr auto parse(format_parse_context &ctx) { return ctx.begin(); }
    auto format(const Rule &rule, format_context &ctx) const {
        auto out = ctx.out();

        out = format_to(out, "B");
        for (int i = 0; i <= 8; i++)
            if (rule.birth & (1 << i))
                out = format_to(out, "{}", i);

        out = format_to(out, "/S");
        for (int i = 0; i <= 8; i++)
            if (rule.survive & (1 << i))
                out = format_to(out, "{}", i);

        return out;
    }
};
