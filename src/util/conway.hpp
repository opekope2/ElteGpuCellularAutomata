#pragma once

#include <CL/cl_platform.h>
#include <format>

typedef struct ConwayRule {
    cl_ushort birth;
    cl_ushort survive;
} conway_rule_t;

template <>
struct std::formatter<conway_rule_t> {
    constexpr auto parse(format_parse_context &ctx) { return ctx.begin(); }
    auto format(const conway_rule_t &rule, format_context &ctx) const {
        auto out = format_to(ctx.out(), "B");
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
