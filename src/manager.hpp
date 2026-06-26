#pragma once

#include "cellular_automaton.hpp"
#include "conway.hpp"
#include "langton.hpp"
#include "state.hpp"
#include <CL/cl_platform.h>
#include <CL/opencl.hpp>
#include <vector>

using namespace std;
using namespace cl;

class Manager {
private:
    CommandQueue &_q;

    State &_state;

    conway::ConwayCellularAutomaton _conway;
    langton::LangtonCellularAutomaton _langton;
    CellularAutomaton *_automaton = &_conway;
    std::vector<CellularAutomaton *> _automatons = {&_conway, &_langton};

public:
    Manager(Context &ctx, CommandQueue &q, State &state)
        : _q(q),
          _state(state),
          _conway(ctx),
          _langton(ctx) {}

    CommandQueue &queue() { return _q; }

    State &state() { return _state; }

    CellularAutomaton *conway() { return &_conway; }
    CellularAutomaton *langton() { return &_langton; }
    CellularAutomaton *automaton() { return _automaton; }
    std::vector<CellularAutomaton *> &automatons() { return _automatons; }

    void automaton(CellularAutomaton *automaton) { _automaton = automaton; }
};
