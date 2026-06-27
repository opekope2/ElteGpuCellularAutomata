#pragma once

#include "../gen/kernels.hpp"
#include "cellular_automaton.hpp"
#include "util/cellular_automaton.hpp"
#include "util/cl.hpp"
#include "util/misc.hpp"
#include "util/turmite.hpp"
#include <CL/cl.h>
#include <CL/cl_platform.h>
#include <CL/opencl.hpp>
#include <exception>
#include <format>

using namespace cl;

namespace turmite {

TurmiteRuleset parseRule(string &rule);

class TurmiteCellularAutomaton : public CellularAutomaton {
private:
    KernelFunctor<cl_uint, cl_uint, Buffer, Buffer, cell_t, Buffer, Buffer> _turmiteStep;

    TurmiteRuleset _ruleset{{{1, TURN_RIGHT, 0}, {0, TURN_LEFT, 0}}, 2, 1};

    Buffer _ant;
    Buffer _state;
    Buffer _rules;

public:
    TurmiteCellularAutomaton(Context &ctx)
        : CellularAutomaton(ctx, buildProgram(ctx, cl::Program::Sources{XXD_STRING(cellular_automaton_cl), XXD_STRING(turmite_cl)})),
          _turmiteStep(program, "turmiteStep"),
          _ant(ctx, CL_MEM_READ_WRITE, sizeof(ant_t)),
          _state(ctx, CL_MEM_READ_WRITE, sizeof(state_t)),
          _rules(ctx, CL_MEM_READ_WRITE, sizeof(turmite_rule_t) * MAX_STATE * MAX_VALUE) {
    }

    bool doubleBuffered() override { return false; }
    string name() override { return "Turmite"; }
    string sampleData() override { return ""; }
    string rule() override { return format("{}", _ruleset); }
    bool rule(CommandQueue &q, string rule) override {
        try {
            return _ruleset = parseRule(rule), true;
        } catch (exception &) {
            return false;
        }
    }

    void init(CommandQueue &q, State &state, std::vector<Event> &events) override {
        q.enqueueFillBuffer<ant_t>(_ant, ant_t{0, FACING_NORTH}, 0, sizeof(ant_t));
        q.enqueueFillBuffer<state_t>(_state, 0, 0, sizeof(state_t));
        q.enqueueWriteBuffer(_rules, CL_FALSE, 0, sizeof(turmite_rule_t) * _ruleset.nStates * _ruleset.nCellValues, _ruleset.rules.data());
        q.enqueueFillBuffer<cell_t>(state.current(), ANT_MASK, 0, sizeof(cell_t));
        q.enqueueFillBuffer<cell_t>(state.current(), 0, sizeof(cell_t), sizeof(cell_t) * (state.width() * state.height() - 1));
    }

    bool load(CommandQueue &q, State &state, cl_uint x, cl_uint y, string &data, std::vector<Event> &events) override { return true; } // Returning false would instantly fail the benchmark

    void step(CommandQueue &q, State &state, std::vector<Event> &events) override {
        Event stepEvent = _turmiteStep(
            EnqueueArgs(q, NDRange(1)),
            state.width(),
            state.height(),
            _ant,
            _state,
            _ruleset.nCellValues,
            _rules,
            state.current());

        events.push_back(stepEvent);
    }
};

} // namespace turmite
