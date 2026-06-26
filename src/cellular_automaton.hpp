#pragma once

#include "state.hpp"
#include <CL/cl_platform.h>
#include <CL/opencl.hpp>
#include <vector>

using namespace cl;

class CellularAutomaton {
protected:
    Context &ctx;
    Program program;

public:
    CellularAutomaton(Context &ctx, Program program) : ctx(ctx), program(program) {}

    virtual bool doubleBuffered() { return true; }
    virtual string name() = 0;
    virtual string sampleData() = 0;
    virtual void init(CommandQueue &q, State &state, std::vector<Event> &events) = 0;
    virtual bool load(CommandQueue &q, State &state, cl_uint x, cl_uint y, string &data, std::vector<Event> &events) = 0;
    virtual void step(CommandQueue &q, State &state, std::vector<Event> &events) = 0;
};
