#pragma once

#if defined(GUI)

#include "../gen/kernels.hpp"
#include "../gen/shaders.hpp"
#include "manager.hpp"
#include "state.hpp"
#include "util/cl.hpp"
#include "util/gl.hpp"
#include "util/misc.hpp"
#include <CL/opencl.hpp>
#include <algorithm>
#include <epoxy/gl.h>

#define VERTEX_COUNT (4 * 2)

using namespace std;
using namespace cl;

extern float fullQuadVerts[VERTEX_COUNT];

class Renderer {
private:
    GlVertexArray _vao;
    GlBuffer _vbo;
    GlProgram _conwayProgram;
    GlProgram _langtonProgram;
    Program _clProgram;
    KernelFunctor<Buffer, ImageGL> _render;

    GlState &_state;

    GLint _offsetX = 0;
    GLint _offsetY = 0;
    GLint _zoom = 1;

public:
    Renderer(Context &ctx, GlState &state)
        : _vao(createVertexArray()),
          _vbo(createBuffer()),
          _conwayProgram(createShaderProgram(reinterpret_cast<char *>(conway_vert), reinterpret_cast<char *>(conway_frag))),
          _langtonProgram(createShaderProgram(reinterpret_cast<char *>(langton_vert), reinterpret_cast<char *>(langton_frag))),
          _clProgram(buildProgram(ctx, cl::Program::Sources{XXD_STRING(cellular_automaton_cl), XXD_STRING(render_cl)})),
          _render(_clProgram, "render"),
          _state(state) {
        glBindVertexArray(_vao);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0); // vec2 pos
        glEnableVertexAttribArray(0);

        glNamedBufferData(_vbo, sizeof(fullQuadVerts), fullQuadVerts, GL_STATIC_DRAW);
    }

    GLint offsetX() { return _offsetX; }
    GLint offsetY() { return _offsetY; }
    void offsetX(GLint offset) { _offsetX = (offset % _state.width() + _state.width()) % _state.width(); }
    void offsetY(GLint offset) { _offsetY = (offset % _state.height() + _state.height()) % _state.height(); }

    GLint zoom() { return _zoom; }
    void zoom(GLint zoom) { _zoom = max(zoom, 1); }

    void renderData(CommandQueue &q) {
        q.enqueueAcquireGLObjects(&_state.glObjs());
        _render(
            EnqueueArgs(q, NDRange(_state.width(), _state.height())),
            _state.current(),
            _state.glImage());
        q.enqueueReleaseGLObjects(&_state.glObjs());
        q.finish();
    }

    void render(int w, int h, Manager &manager, GlTexture &data) {
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        // TODO make polymorphic
        GlProgram &program = manager.automaton() == manager.conway() ? _conwayProgram : _langtonProgram;

        glUseProgram(program);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, data);

        glUniform1i(glGetUniformLocation(program, "data"), 0);
        glUniform2i(glGetUniformLocation(program, "res"), w, h);
        glUniform2i(glGetUniformLocation(program, "offset"), _offsetX, _offsetY);
        glUniform1i(glGetUniformLocation(program, "zoom"), _zoom);

        glBindVertexArray(_vao);
        glDrawArrays(GL_TRIANGLE_FAN, 0, VERTEX_COUNT);
    }
};

#endif
