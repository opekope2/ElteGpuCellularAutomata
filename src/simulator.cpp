#include "simulator.hpp"
#include "cellular_automaton.hpp"
#include "manager.hpp"
#include "renderer.hpp"
#include "state.hpp"
#include "util/glfw.hpp"
#include "util/misc.hpp"
#include <CL/cl.h>
#include <CL/cl_platform.h>
#include <CL/opencl.hpp>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <format>
#include <iostream>
#include <numeric>
#include <vector>

using namespace std;
using namespace cl;

#if defined(GUI)
#include <GLFW/glfw3.h>
#include <epoxy/gl.h>

void handleInput(GLFWwindow *window, int key, int scancode, int action, int mods) {
    auto *renderer = static_cast<Renderer *>(glfwGetWindowUserPointer(window));

    cl_uint dx = 0, dy = 0, zoom = 0;
    uint8_t speed = renderer->speed();

    if (key == GLFW_KEY_LEFT && action != GLFW_RELEASE)
        dx -= AMOUNT(mods);
    if (key == GLFW_KEY_RIGHT && action != GLFW_RELEASE)
        dx += AMOUNT(mods);
    if (key == GLFW_KEY_UP && action != GLFW_RELEASE)
        dy += AMOUNT(mods);
    if (key == GLFW_KEY_DOWN && action != GLFW_RELEASE)
        dy -= AMOUNT(mods);

    if (key == GLFW_KEY_EQUAL && action != GLFW_RELEASE)
        zoom += AMOUNT(mods);
    if (key == GLFW_KEY_MINUS && action != GLFW_RELEASE)
        zoom -= AMOUNT(mods);

    if (key == GLFW_KEY_SPACE && action != GLFW_RELEASE)
        speed = !speed;
    if (key == GLFW_KEY_LEFT_BRACKET && action != GLFW_RELEASE)
        speed -= AMOUNT(mods);
    if (key == GLFW_KEY_RIGHT_BRACKET && action != GLFW_RELEASE)
        speed += AMOUNT(mods);

    if (key == GLFW_KEY_Q && action != GLFW_RELEASE)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    if (zoom)
        renderer->zoom(renderer->zoom() + zoom);
    if (dx || dy)
        renderer->offsetX(renderer->offsetX() + dx), renderer->offsetY(renderer->offsetY() - dy);
    if (speed != renderer->speed())
        renderer->speed(speed);
}

void cellularAutomatonGui(GlfwWindow &win, Context &ctx, Manager &manager) {
    GlState &state = dynamic_cast<GlState &>(manager.state());
    CommandQueue &q = manager.queue();
    Renderer renderer(ctx, state);

    std::vector<Event> events;
    manager.automaton()->init(q, state, events);
    events.clear();

    glfwSetWindowTitle(win, manager.automaton()->name().c_str());
    glfwSetWindowUserPointer(win, &renderer);
    glfwSetKeyCallback(win, handleInput);

    while (!glfwWindowShouldClose(win)) {
        int w, h;
        glfwGetFramebufferSize(win, &w, &h);
        glViewport(0, 0, w, h);

        renderer.renderData(q);
        renderer.render(w, h, state.texture());

        glfwSwapBuffers(win);
        glfwPollEvents();

        for (uint8_t i = renderer.speed(); i > 0; i--) {
            std::vector<Event> events;
            manager.automaton()->step(q, state, events);
            state.swapBuffers();
        }
    }
}
#endif
