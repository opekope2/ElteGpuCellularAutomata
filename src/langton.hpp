#pragma once

#include "../gen/kernels.hpp"
#include "cellular_automaton.hpp"
#include "util/cellular_automaton.hpp"
#include "util/cl.hpp"
#include "util/misc.hpp"
#include <CL/cl.h>
#include <CL/cl_platform.h>
#include <CL/opencl.hpp>

#define WHITE 0x01
#define ANT 0x02
#define FACING (0x04 | 0x08)
#define FACING_OFFSET 0x04

#define FACING_NORTH (0 * FACING_OFFSET)
#define FACING_EAST (1 * FACING_OFFSET)
#define FACING_SOUTH (2 * FACING_OFFSET)
#define FACING_WEST (3 * FACING_OFFSET)

using namespace cl;

namespace langton {

class LangtonCellularAutomaton : public CellularAutomaton {
private:
    KernelFunctor<cl_uint, cl_uint, Buffer, Buffer> langtonStep;

    Buffer antXY;

public:
    LangtonCellularAutomaton(Context &ctx)
        : CellularAutomaton(ctx, buildProgram(ctx, cl::Program::Sources{XXD_STRING(cellular_automaton_cl), XXD_STRING(langton_cl)})),
          langtonStep(program, "langtonStep"),
          antXY(ctx, CL_MEM_READ_WRITE, sizeof(cl_uint)) {}

    bool doubleBuffered() override { return false; }

    string name() override { return "Langton's Ant"; }

    string sampleData() override { return ""; }

    void init(CommandQueue &q, State &state, std::vector<Event> &events) override {
        q.enqueueFillBuffer<cell_t>(state.current(), WHITE | ANT | FACING_WEST, 0, sizeof(cell_t));
        q.enqueueFillBuffer<cell_t>(state.current(), WHITE, sizeof(cell_t), sizeof(cell_t) * (state.width() * state.height() - 1));
        q.enqueueFillBuffer<cl_uint>(antXY, 0, 0, sizeof(cl_uint));
    }

    bool load(CommandQueue &q, State &state, cl_uint x, cl_uint y, string &data, std::vector<Event> &events) override { return true; }

    void step(CommandQueue &q, State &state, std::vector<Event> &events) override {
        Event stepEvent = langtonStep(
            EnqueueArgs(q, NDRange(1)),
            state.width(),
            state.height(),
            antXY,
            state.current());

        events.push_back(stepEvent);
    }
};

} // namespace langton
