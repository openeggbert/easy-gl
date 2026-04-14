#pragma once

#include "easygl/Types.hpp"

namespace easygl::platform
{
    // Minimal GL types for internal pointers
    using GLsizei = int;
    using GLuint = unsigned int;
    using GLint = int;
    using GLenum = unsigned int;

    // Typedefs for a few GL functions to demonstrate the loader
    using PFNGLGENBUFFERSPROC = void (*)(GLsizei n, GLuint* buffers);
    using PFNGLDELETEBUFFERSPROC = void (*)(GLsizei n, const GLuint* buffers);
    using PFNGLBINDBUFFERPROC = void (*)(GLenum target, GLuint buffer);
    using PFNGLGETSTRINGPROC = const unsigned char* (*)(GLenum name);
    using PFNGLGETINTEGERVPROC = void (*)(GLenum pname, GLint* data);

    struct GlFunctions
    {
        GLGetProcAddressFn get_proc_address = nullptr;

        bool initialized = false;

        // Pointers to OpenGL functions
        PFNGLGENBUFFERSPROC GenBuffers = nullptr;
        PFNGLDELETEBUFFERSPROC DeleteBuffers = nullptr;
        PFNGLBINDBUFFERPROC BindBuffer = nullptr;
        PFNGLGETSTRINGPROC GetString = nullptr;
        PFNGLGETINTEGERVPROC GetIntegerv = nullptr;
    };

    bool initialize_gl_functions(GlFunctions& functions, GLGetProcAddressFn callback);

    // Global instance used by the library internally
    extern GlFunctions g_gl;
}