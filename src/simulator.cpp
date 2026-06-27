#include "simulator.hpp"
#include "benchmark.hpp"
#include "cellular_automaton.hpp"
#include "conway.hpp"
#include "manager.hpp"
#include "renderer.hpp"
#include "state.hpp"
#include "util/cl.hpp"
#include "util/conway.hpp"
#include "util/glfw.hpp"
#include "util/misc.hpp"
#include <CL/cl.h>
#include <CL/cl_platform.h>
#include <CL/opencl.hpp>
#include <cstdint>
#include <format>
#include <iostream>
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

bool load(Manager &manager, string data, std::vector<Event> &events) {
    manager.automaton()->init(manager.queue(), manager.state(), events);
    bool success = manager.automaton()->load(manager.queue(), manager.state(), 0, 0, data, events);
    manager.queue().finish();

    if (success)
        return true;
    return cerr << "Failed to load data: " << data << endl, false;
}

bool loadRule(Manager &manager, string data, std::vector<Event> &events) {
    bool success = manager.automaton()->rule(manager.queue(), data);
    manager.queue().finish();

    if (success)
        return load(manager, manager.automaton()->sampleData(), events);
    return cerr << "Failed to load rule: " << data << endl, false;
}

void step(Manager &manager, std::vector<Event> &events) {
    if (manager.automaton()->doubleBuffered())
        manager.state().swapBuffers();
    manager.automaton()->step(manager.queue(), manager.state(), events);
    manager.queue().finish();
}

void updateTitle(GLFWwindow *win, UserPointer *data) {
    Manager &manager = data->manager;
    string rule = manager.automaton()->rule();
    string title = rule.size() > 128
                       ? format("{} [{}...{}] (x{})", manager.automaton()->name(), rule.substr(0, 64), rule.substr(rule.size() - 64), data->speed)
                       : format("{} [{}] (x{})", manager.automaton()->name(), rule, data->speed);
    glfwSetWindowTitle(win, title.c_str());
}

void handleInput(GLFWwindow *window, int key, int scancode, int action, int mods) {
    UserPointer *data = static_cast<UserPointer *>(glfwGetWindowUserPointer(window));
    Manager &manager = data->manager;
    Renderer &renderer = data->renderer;
    std::vector<Event> events;

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
    if (key == GLFW_KEY_PERIOD && action != GLFW_RELEASE && !data->speed)
        for (uint8_t i = AMOUNT(mods); i; i--)
            step(manager, events);

    if (key == GLFW_KEY_C && action != GLFW_RELEASE)
        manager.automaton(manager.conway()), load(manager, manager.automaton()->sampleData(), events);
    if (key == GLFW_KEY_T && action != GLFW_RELEASE)
        manager.automaton(manager.turmite()), load(manager, manager.automaton()->sampleData(), events);

    if (key == GLFW_KEY_V && action != GLFW_RELEASE && mods == GLFW_MOD_CONTROL) {
        data->speed = 0;
        renderer.offsetX(0), renderer.offsetY(0);
        auto clipboard = glfwGetClipboardString(window);
        if (clipboard != nullptr)
            load(manager, clipboard, events);
    }
    if (key == GLFW_KEY_B && action != GLFW_RELEASE && mods == GLFW_MOD_CONTROL) {
        auto clipboard = glfwGetClipboardString(window);
        if (clipboard != nullptr)
            loadRule(manager, clipboard, events); // B36/S23
    }

    if (auto *conwayAutomaton = dynamic_cast<conway::ConwayCellularAutomaton *>(manager.automaton())) {
        conway_rule_t &conwayRule = conwayAutomaton->conwayRule();
        for (int i = 0; i <= 8; i++)
            if (key == (GLFW_KEY_0 + i) && action != GLFW_RELEASE) {
                if (mods == GLFW_MOD_CONTROL)
                    conwayRule.birth ^= 1 << i;
                else if (mods == GLFW_MOD_SHIFT)
                    conwayRule.survive ^= 1 << i;
            }
    }

    if (key == GLFW_KEY_Q && action != GLFW_RELEASE)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    if (zoom)
        renderer.zoom(renderer.zoom() + zoom);
    if (dx || dy)
        renderer.offsetX(renderer.offsetX() + dx), renderer.offsetY(renderer.offsetY() - dy);

    updateTitle(window, data);
}

void cellularAutomatonGui(GlfwWindow &win, Context &ctx, Manager &manager) {
    GlState &state = dynamic_cast<GlState &>(manager.state());
    CommandQueue &q = manager.queue();
    Renderer renderer(ctx, state);
    UserPointer data{manager, renderer};

    {
        std::vector<Event> events;
        load(manager, manager.automaton()->sampleData(), events);
    }

    updateTitle(win, &data);
    glfwSetWindowUserPointer(win, &data);
    glfwSetKeyCallback(win, handleInput);

    while (!glfwWindowShouldClose(win)) {
        int w, h;
        glfwGetFramebufferSize(win, &w, &h);
        glViewport(0, 0, w, h);

        renderer.renderData(q);
        renderer.render(w, h, manager, state.texture());

        glfwSwapBuffers(win);
        glfwPollEvents();

        std::vector<Event> events;
        for (uint8_t i = data.speed; i > 0; i--)
            step(manager, events);
    }
}
#endif

void cellularAutomatonBenchmark(Manager &manager, benchmark::Benchmark &benchmark) {
    cout << benchmark::HEADER << endl;

    {
        std::vector<Event> events;
        if (!loadRule(manager, benchmark.rule, events))
            return;
        if (!load(manager, benchmark.data, events))
            return;
    }

    for (int i = 1; i <= benchmark.generations; i++) {
        std::vector<Event> events;
        step(manager, events);
        cl_ulong ns = getProfilingTimeNs(events);
        benchmark::printResult(cout, benchmark, i, ns);
    }
}
