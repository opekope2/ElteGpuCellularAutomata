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
#include <string>
#include <vector>

using namespace std;
using namespace cl;

#if defined(GUI)
#include <GLFW/glfw3.h>
#include <epoxy/gl.h>

struct UserPointer {
    Manager &manager;
    Renderer &renderer;
    uint8_t speed = 0;
};

void load(Manager &manager, CellularAutomaton *automaton, string data) {
    std::vector<Event> events;
    automaton->init(manager.queue(), manager.state(), events);
    automaton->load(manager.queue(), manager.state(), 0, 0, data, events);
}

void handleInput(GLFWwindow *window, int key, int scancode, int action, int mods) {
    UserPointer *data = static_cast<UserPointer *>(glfwGetWindowUserPointer(window));
    Manager &manager = data->manager;
    Renderer &renderer = data->renderer;

    cl_uint dx = 0, dy = 0, zoom = 0;

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
        data->speed = !data->speed;
    if (key == GLFW_KEY_LEFT_BRACKET && action != GLFW_RELEASE)
        data->speed -= AMOUNT(mods);
    if (key == GLFW_KEY_RIGHT_BRACKET && action != GLFW_RELEASE)
        data->speed += AMOUNT(mods);

    if (key == GLFW_KEY_V && action != GLFW_RELEASE && mods == GLFW_MOD_CONTROL) {
        data->speed = 0;
        renderer.offsetX(0), renderer.offsetY(0);
        auto clipboard = glfwGetClipboardString(window);
        if (clipboard != nullptr)
            load(manager, manager.automaton(), clipboard);
    }

    if (key == GLFW_KEY_Q && action != GLFW_RELEASE)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    if (zoom)
        renderer.zoom(renderer.zoom() + zoom);
    if (dx || dy)
        renderer.offsetX(renderer.offsetX() + dx), renderer.offsetY(renderer.offsetY() - dy);
}

void cellularAutomatonGui(GlfwWindow &win, Context &ctx, Manager &manager) {
    GlState &state = dynamic_cast<GlState &>(manager.state());
    CommandQueue &q = manager.queue();
    Renderer renderer(ctx, state);
    UserPointer data{manager, renderer};

    load(manager, manager.automaton(), manager.automaton()->sampleData());

    glfwSetWindowTitle(win, manager.automaton()->name().c_str());
    glfwSetWindowUserPointer(win, &data);
    glfwSetKeyCallback(win, handleInput);

    while (!glfwWindowShouldClose(win)) {
        int w, h;
        glfwGetFramebufferSize(win, &w, &h);
        glViewport(0, 0, w, h);

        renderer.renderData(q);
        renderer.render(w, h, state.texture());

        glfwSwapBuffers(win);
        glfwPollEvents();

        for (uint8_t i = data.speed; i > 0; i--) {
            std::vector<Event> events;
            state.swapBuffers();
            manager.automaton()->step(q, state, events);
        }
    }
}
#endif
