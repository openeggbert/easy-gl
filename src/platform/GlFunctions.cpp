#include "GlFunctions.hpp"

namespace easygl::platform
{
    GlFunctions g_gl;

    namespace {
        template<typename T>
        T load_func(GLGetProcAddressFn loader, const char* name)
        {
            // Casting void* to function pointer is standard practice in GL loading
            return reinterpret_cast<T>(loader(name));
        }
    }

    bool initialize_gl_functions(GlFunctions& functions, GLGetProcAddressFn callback)
    {
        if (callback == nullptr)
        {
            return false;
        }

        functions.get_proc_address = callback;
        
        // Load some functions for demo
        functions.GenBuffers = load_func<PFNGLGENBUFFERSPROC>(callback, "glGenBuffers");
        functions.DeleteBuffers = load_func<PFNGLDELETEBUFFERSPROC>(callback, "glDeleteBuffers");
        functions.BindBuffer = load_func<PFNGLBINDBUFFERPROC>(callback, "glBindBuffer");
        functions.GetString = load_func<PFNGLGETSTRINGPROC>(callback, "glGetString");
        functions.GetIntegerv = load_func<PFNGLGETINTEGERVPROC>(callback, "glGetIntegerv");

        functions.initialized = true;
        return true;
    }
}