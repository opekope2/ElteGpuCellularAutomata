#pragma once

#include "../gen/kernels.hpp"
#include "cellular_automaton.hpp"
#include "util/cellular_automaton.hpp"
#include "util/cl.hpp"
#include "util/conway.hpp"
#include "util/misc.hpp"
#include <CL/cl.h>
#include <CL/cl_platform.h>
#include <CL/opencl.hpp>
#include <cstddef>
#include <format>

using namespace std;

namespace conway {

const string GOSPER_GLIDER_GUN = "24bo11b$22bobo11b$12b2o6b2o12b2o$11bo3bo4b2o12b2o$2o8bo5bo3b2o14b$2o8bo3bob2o4bobo11b$10bo5bo7bo11b$11bo3bo20b$12b2o!";

class ConwayCellularAutomaton : public CellularAutomaton {
private:
    KernelFunctor<conway_rule_t, Buffer, Buffer> conwayStep;
    KernelFunctor<cl_uint, cl_uint, cl_uint, Buffer, Buffer> loadRle;

    conway_rule_t _rule{1 << 3, 1 << 2 | 1 << 3};

public:
    ConwayCellularAutomaton(Context &ctx)
        : CellularAutomaton(ctx, buildProgram(ctx, cl::Program::Sources{XXD_STRING(cellular_automaton_cl), XXD_STRING(conway_cl)})),
          conwayStep(program, "conwayStep"),
          loadRle(program, "loadRle") {}

    string name() override { return "Conway's Game of Life"; }

    string sampleData() override { return GOSPER_GLIDER_GUN; }

    string rule() override { return format("{}", _rule); }

    bool rule(CommandQueue &q, string rule) override {
        conway_rule_t r{0, 0};

        if (rule.size() == 0 || rule[0] != 'B')
            return false;

        size_t i;
        for (i = 1; i < rule.size(); i++) {
            char c = rule[i];
            if (c == '/')
                break;
            if (c < '0' || c > '8')
                return false;
            r.birth |= 1 << (c - '0');
        }

        if (++i >= rule.size() || rule[i] != 'S')
            return false;

        for (i++; i < rule.size(); i++) {
            char c = rule[i];
            if (c < '0' || c > '8')
                return false;
            r.survive |= 1 << (c - '0');
        }

        _rule = r;
        return true;
    }

    conway_rule_t &conwayRule() { return _rule; }

    void init(CommandQueue &q, State &state, std::vector<Event> &events) override {
        q.enqueueFillBuffer<cell_t>(state.current(), (cell_t)0, 0, sizeof(cell_t) * state.width() * state.height());
    }

    bool load(CommandQueue &q, State &state, cl_uint x, cl_uint y, string &data, std::vector<Event> &events) override {
        if (data.rfind('!') == string::npos)
            return false;

        Buffer dataBuf(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, data.size(), (void *)data.c_str());
        Event loadCellsEvent = loadRle(
            EnqueueArgs(q, NDRange(1)),
            state.width(),
            x,
            y,
            dataBuf,
            state.current());

        events.push_back(loadCellsEvent);

        return true;
    }

    void step(CommandQueue &q, State &state, std::vector<Event> &events) override {
        Event stepEvent = conwayStep(
            EnqueueArgs(q, NDRange(state.width(), state.height())),
            _rule,
            state.previous(),
            state.current());

        events.push_back(stepEvent);
    }
};

} // namespace conway
