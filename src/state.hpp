#pragma once

#include "util/cellular_automaton.hpp"
#include "util/gl.hpp"
#include <CL/cl.h>
#include <CL/cl_platform.h>
#include <CL/opencl.hpp>
#include <cassert>
#include <utility>
#include <vector>

using namespace cl;

class State {
protected:
    cl_uint _width;
    cl_uint _height;

    Context &_ctx;

    Buffer _prev;
    Buffer _current;

public:
    State(Context &ctx, CommandQueue &q, cl_uint width, cl_uint height)
        : _width(width),
          _height(height),
          _ctx(ctx),
          _prev(_ctx, CL_MEM_READ_WRITE, sizeof(cell_t) * width * height),
          _current(_ctx, CL_MEM_READ_WRITE, sizeof(cell_t) * width * height) {}

    cl_uint width() { return _width; }
    cl_uint height() { return _height; }

    Buffer &previous() { return _prev; }
    Buffer &current() { return _current; }

    void swapBuffers() { swap(_prev, _current); }

    // Virtul method so dynamic_cast<GlState &> works
    virtual void polymorphic() {}
};

#if defined(GUI)
#include <epoxy/gl.h>

class GlState : public State {
private:
    GlTexture _tex;
    ImageGL _glImg;
    std::vector<Memory> _glObjs;

public:
    GlState(Context &ctx, CommandQueue &q, cl_uint width, cl_uint height) : State(ctx, q, width, height), _tex(createTexture<GL_TEXTURE_2D>()) {
        glTextureStorage2D(_tex, 1, GL_R8UI, width, height);
        glTextureParameteri(_tex, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(_tex, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        _glImg = ImageGL(_ctx, CL_MEM_WRITE_ONLY, GL_TEXTURE_2D, 0, _tex);
        _glObjs.push_back(_glImg);
    }

    GlTexture &texture() { return _tex; }
    ImageGL &glImage() { return _glImg; }
    std::vector<Memory> &glObjs() { return _glObjs; }
};
#endif
