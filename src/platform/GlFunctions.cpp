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
        
        // Buffers
        functions.GenBuffers = load_func<PFNGLGENBUFFERSPROC>(callback, "glGenBuffers");
        functions.DeleteBuffers = load_func<PFNGLDELETEBUFFERSPROC>(callback, "glDeleteBuffers");
        functions.BindBuffer = load_func<PFNGLBINDBUFFERPROC>(callback, "glBindBuffer");
        functions.BufferData = load_func<PFNGLBUFFERDATAPROC>(callback, "glBufferData");
        functions.BindBufferBase = load_func<PFNGLBINDBUFFERBASEPROC>(callback, "glBindBufferBase");

        // Context info
        functions.GetString = load_func<PFNGLGETSTRINGPROC>(callback, "glGetString");
        functions.GetIntegerv = load_func<PFNGLGETINTEGERVPROC>(callback, "glGetIntegerv");

        // Shaders
        functions.CreateShader = load_func<PFNGLCREATESHADERPROC>(callback, "glCreateShader");
        functions.ShaderSource = load_func<PFNGLSHADERSOURCEPROC>(callback, "glShaderSource");
        functions.CompileShader = load_func<PFNGLCOMPILESHADERPROC>(callback, "glCompileShader");
        functions.GetShaderiv = load_func<PFNGLGETSHADERIVPROC>(callback, "glGetShaderiv");
        functions.GetShaderInfoLog = load_func<PFNGLGETSHADERINFOLOGPROC>(callback, "glGetShaderInfoLog");
        functions.DeleteShader = load_func<PFNGLDELETESHADERPROC>(callback, "glDeleteShader");

        // Programs
        functions.CreateProgram = load_func<PFNGLCREATEPROGRAMPROC>(callback, "glCreateProgram");
        functions.AttachShader = load_func<PFNGLATTACHSHADERPROC>(callback, "glAttachShader");
        functions.DetachShader = load_func<PFNGLDETACHSHADERPROC>(callback, "glDetachShader");
        functions.LinkProgram = load_func<PFNGLLINKPROGRAMPROC>(callback, "glLinkProgram");
        functions.GetProgramiv = load_func<PFNGLGETPROGRAMIVPROC>(callback, "glGetProgramiv");
        functions.GetProgramInfoLog = load_func<PFNGLGETPROGRAMINFOLOGPROC>(callback, "glGetProgramInfoLog");
        functions.UseProgram = load_func<PFNGLUSEPROGRAMPROC>(callback, "glUseProgram");
        functions.DeleteProgram = load_func<PFNGLDELETEPROGRAMPROC>(callback, "glDeleteProgram");

        // Uniforms
        functions.GetUniformLocation = load_func<PFNGLGETUNIFORMLOCATIONPROC>(callback, "glGetUniformLocation");
        functions.Uniform1i = load_func<PFNGLUNIFORM1IPROC>(callback, "glUniform1i");
        functions.Uniform1f = load_func<PFNGLUNIFORM1FPROC>(callback, "glUniform1f");
        functions.Uniform3f = load_func<PFNGLUNIFORM3FPROC>(callback, "glUniform3f");
        functions.UniformMatrix4fv = load_func<PFNGLUNIFORMMATRIX4FVPROC>(callback, "glUniformMatrix4fv");

        // Vertex Arrays
        functions.GenVertexArrays = load_func<PFNGLGENVERTEXARRAYSPROC>(callback, "glGenVertexArrays");
        functions.DeleteVertexArrays = load_func<PFNGLDELETEVERTEXARRAYSPROC>(callback, "glDeleteVertexArrays");
        functions.BindVertexArray = load_func<PFNGLBINDVERTEXARRAYPROC>(callback, "glBindVertexArray");
        functions.EnableVertexAttribArray = load_func<PFNGLENABLEATTRIBARRAYPROC>(callback, "glEnableVertexAttribArray");
        functions.VertexAttribPointer = load_func<PFNGLVERTEXATTRIBPOINTERPROC>(callback, "glVertexAttribPointer");

        // Commands
        functions.Clear = load_func<PFNGLCLEARPROC>(callback, "glClear");
        functions.ClearColor = load_func<PFNGLCLEARCOLORPROC>(callback, "glClearColor");
        functions.Viewport = load_func<PFNGLVIEWPORTPROC>(callback, "glViewport");
        functions.DrawArrays = load_func<PFNGLDRAWARRAYSPROC>(callback, "glDrawArrays");

        functions.initialized = true;
        return true;
    }
}