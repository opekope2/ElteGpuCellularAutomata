#pragma once

#include "cellular_automaton.hpp"
#include "conway.hpp"
#include "state.hpp"
#include "turmite.hpp"
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
    turmite::TurmiteCellularAutomaton _turmite;
    CellularAutomaton *_automaton = &_conway;
    std::vector<CellularAutomaton *> _automatons = {&_conway, &_turmite};

public:
    Manager(Context &ctx, CommandQueue &q, State &state)
        : _q(q),
          _state(state),
          _conway(ctx),
          _turmite(ctx) {}

    CommandQueue &queue() { return _q; }

    State &state() { return _state; }

    CellularAutomaton *conway() { return &_conway; }
    CellularAutomaton *turmite() { return &_turmite; }
    CellularAutomaton *automaton() { return _automaton; }
    std::vector<CellularAutomaton *> &automatons() { return _automatons; }

    void automaton(CellularAutomaton *automaton) { _automaton = automaton; }
};
