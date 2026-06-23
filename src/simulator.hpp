#pragma once

#include "manager.hpp"
#include "util/glfw.hpp"

#if defined(GUI)
void cellularAutomatonGui(GlfwWindow &win, cl::Context &ctx, Manager &manager);
#endif
