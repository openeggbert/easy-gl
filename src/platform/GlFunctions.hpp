#pragma once

#include "easygl/Types.hpp"

namespace easygl::platform
{
    // Minimal GL types for internal pointers
    using GLsizei = int;
    using GLuint = unsigned int;
    using GLint = int;
    using GLenum = unsigned int;
    using GLchar = char;
    using GLboolean = unsigned char;
    using GLbitfield = unsigned int;
    using GLfloat = float;
    using GLdouble = double;
    using GLsizeiptr = long;
    using GLintptr = long;

    // GL constants
    constexpr GLenum GL_VENDOR = 0x1F00;
    constexpr GLenum GL_RENDERER = 0x1F01;
    constexpr GLenum GL_VERSION = 0x1F02;
    constexpr GLenum GL_EXTENSIONS = 0x1F03;
    constexpr GLenum GL_SHADING_LANGUAGE_VERSION = 0x8B8C;
    constexpr GLenum GL_MAJOR_VERSION = 0x821B;
    constexpr GLenum GL_MINOR_VERSION = 0x821C;
    constexpr GLenum GL_NUM_EXTENSIONS = 0x821D;
    constexpr GLenum GL_MAX_TEXTURE_SIZE = 0x0D33;

    constexpr GLenum GL_TEXTURE_2D = 0x0DE1;
    constexpr GLenum GL_TEXTURE_MAG_FILTER = 0x2800;
    constexpr GLenum GL_TEXTURE_MIN_FILTER = 0x2801;
    constexpr GLenum GL_NEAREST = 0x2600;
    constexpr GLenum GL_LINEAR = 0x2601;
    constexpr GLenum GL_RGBA = 0x1908;
    constexpr GLenum GL_BGRA = 0x80E1;

    constexpr GLenum GL_BLEND = 0x0BE2;
    constexpr GLenum GL_SRC_ALPHA = 0x0302;
    constexpr GLenum GL_ONE_MINUS_SRC_ALPHA = 0x0303;

    constexpr GLenum GL_ARRAY_BUFFER = 0x8892;
    constexpr GLenum GL_ELEMENT_ARRAY_BUFFER = 0x8893;
    constexpr GLenum GL_UNIFORM_BUFFER = 0x8A11;
    constexpr GLenum GL_COPY_READ_BUFFER = 0x8F36;
    constexpr GLenum GL_COPY_WRITE_BUFFER = 0x8F37;
    constexpr GLenum GL_STATIC_DRAW = 0x88E4;
    constexpr GLenum GL_DYNAMIC_DRAW = 0x88E8;

    constexpr GLenum GL_VERTEX_SHADER = 0x8B31;
    constexpr GLenum GL_FRAGMENT_SHADER = 0x8B30;
    constexpr GLenum GL_GEOMETRY_SHADER = 0x8DD9;
    constexpr GLenum GL_TESS_CONTROL_SHADER = 0x8E88;
    constexpr GLenum GL_TESS_EVALUATION_SHADER = 0x8E87;
    constexpr GLenum GL_COMPUTE_SHADER = 0x91B9;
    constexpr GLenum GL_COMPILE_STATUS = 0x8B81;
    constexpr GLenum GL_LINK_STATUS = 0x8B82;
    constexpr GLenum GL_INFO_LOG_LENGTH = 0x8B84;

    constexpr GLenum GL_COLOR_BUFFER_BIT = 0x00004000;
    constexpr GLenum GL_DEPTH_BUFFER_BIT = 0x00000100;
    constexpr GLenum GL_STENCIL_BUFFER_BIT = 0x00000400;

    constexpr GLenum GL_TRIANGLES = 0x0004;
    constexpr GLenum GL_TRIANGLE_STRIP = 0x0005;
    constexpr GLenum GL_TRIANGLE_FAN = 0x0006;
    constexpr GLenum GL_LINES = 0x0001;
    constexpr GLenum GL_LINE_STRIP = 0x0003;
    constexpr GLenum GL_LINE_LOOP = 0x0002;
    constexpr GLenum GL_POINTS = 0x0000;

    constexpr GLenum GL_FLOAT = 0x1406;
    constexpr GLenum GL_BYTE = 0x1400;
    constexpr GLenum GL_UNSIGNED_BYTE = 0x1401;
    constexpr GLenum GL_SHORT = 0x1402;
    constexpr GLenum GL_UNSIGNED_SHORT = 0x1403;
    constexpr GLenum GL_INT = 0x1404;
    constexpr GLenum GL_UNSIGNED_INT = 0x1405;

    // Typedefs for GL functions
    using PFNGLGENBUFFERSPROC = void (*)(GLsizei n, GLuint* buffers);
    using PFNGLDELETEBUFFERSPROC = void (*)(GLsizei n, const GLuint* buffers);
    using PFNGLBINDBUFFERPROC = void (*)(GLenum target, GLuint buffer);
    using PFNGLBUFFERDATAPROC = void (*)(GLenum target, GLsizeiptr size, const void* data, GLenum usage);
    using PFNGLBUFFERSUBDATAPROC = void (*)(GLenum target, GLintptr offset, GLsizeiptr size, const void* data);
    using PFNGLBINDBUFFERBASEPROC = void (*)(GLenum target, GLuint index, GLuint buffer);

    using PFNGLGETSTRINGPROC = const unsigned char* (*)(GLenum name);
    using PFNGLGETSTRINGIPROC = const unsigned char* (*)(GLenum name, GLuint index);
    using PFNGLGETINTEGERVPROC = void (*)(GLenum pname, GLint* data);

    using PFNGLCREATESHADERPROC = GLuint (*)(GLenum type);
    using PFNGLSHADERSOURCEPROC = void (*)(GLuint shader, GLsizei count, const GLchar* const* string, const GLint* length);
    using PFNGLCOMPILESHADERPROC = void (*)(GLuint shader);
    using PFNGLGETSHADERIVPROC = void (*)(GLuint shader, GLenum pname, GLint* params);
    using PFNGLGETSHADERINFOLOGPROC = void (*)(GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* infoLog);
    using PFNGLDELETESHADERPROC = void (*)(GLuint shader);

    using PFNGLCREATEPROGRAMPROC = GLuint (*)(void);
    using PFNGLATTACHSHADERPROC = void (*)(GLuint program, GLuint shader);
    using PFNGLDETACHSHADERPROC = void (*)(GLuint program, GLuint shader);
    using PFNGLLINKPROGRAMPROC = void (*)(GLuint program);
    using PFNGLGETPROGRAMIVPROC = void (*)(GLuint program, GLenum pname, GLint* params);
    using PFNGLGETPROGRAMINFOLOGPROC = void (*)(GLuint program, GLsizei bufSize, GLsizei* length, GLchar* infoLog);
    using PFNGLUSEPROGRAMPROC = void (*)(GLuint program);
    using PFNGLDELETEPROGRAMPROC = void (*)(GLuint program);

    using PFNGLGETUNIFORMLOCATIONPROC = GLint (*)(GLuint program, const GLchar* name);
    using PFNGLUNIFORM1IPROC = void (*)(GLint location, GLint v0);
    using PFNGLUNIFORM1FPROC = void (*)(GLint location, GLfloat v0);
    using PFNGLUNIFORM3FPROC = void (*)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
    using PFNGLUNIFORM4FPROC = void (*)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
    using PFNGLUNIFORMMATRIX4FVPROC = void (*)(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);

    using PFNGLGENVERTEXARRAYSPROC = void (*)(GLsizei n, GLuint* arrays);
    using PFNGLDELETEVERTEXARRAYSPROC = void (*)(GLsizei n, const GLuint* arrays);
    using PFNGLBINDVERTEXARRAYPROC = void (*)(GLuint array);
    using PFNGLENABLEATTRIBARRAYPROC = void (*)(GLuint index);
    using PFNGLVERTEXATTRIBPOINTERPROC = void (*)(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer);

    using PFNGLCLEARPROC = void (*)(GLbitfield mask);
    using PFNGLCLEARCOLORPROC = void (*)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
    using PFNGLVIEWPORTPROC = void (*)(GLint x, GLint y, GLsizei width, GLsizei height);
    using PFNGLDRAWARRAYSPROC = void (*)(GLenum mode, GLint first, GLsizei count);
    using PFNGLDRAWELEMENTSPROC = void (*)(GLenum mode, GLsizei count, GLenum type, const void* indices);

    using PFNGLGENTEXTURESPROC = void (*)(GLsizei n, GLuint* textures);
    using PFNGLDELETETEXTURESPROC = void (*)(GLsizei n, const GLuint* textures);
    using PFNGLBINDTEXTUREPROC = void (*)(GLenum target, GLuint texture);
    using PFNGLTEXIMAGE2DPROC = void (*)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void* pixels);
    using PFNGLTEXPARAMETERIPROC = void (*)(GLenum target, GLenum pname, GLint param);

    using PFNGLENABLEPROC = void (*)(GLenum cap);
    using PFNGLDISABLEPROC = void (*)(GLenum cap);
    using PFNGLBLENDFUNCPROC = void (*)(GLenum sfactor, GLenum dfactor);

    struct GlFunctions
    {
        GLGetProcAddressFn get_proc_address = nullptr;

        bool initialized = false;

        // Pointers to OpenGL functions
        PFNGLGENBUFFERSPROC GenBuffers = nullptr;
        PFNGLDELETEBUFFERSPROC DeleteBuffers = nullptr;
        PFNGLBINDBUFFERPROC BindBuffer = nullptr;
        PFNGLBUFFERDATAPROC BufferData = nullptr;
        PFNGLBUFFERSUBDATAPROC BufferSubData = nullptr;
        PFNGLBINDBUFFERBASEPROC BindBufferBase = nullptr;

        PFNGLGETSTRINGPROC GetString = nullptr;
        PFNGLGETSTRINGIPROC GetStringi = nullptr;
        PFNGLGETINTEGERVPROC GetIntegerv = nullptr;

        PFNGLCREATESHADERPROC CreateShader = nullptr;
        PFNGLSHADERSOURCEPROC ShaderSource = nullptr;
        PFNGLCOMPILESHADERPROC CompileShader = nullptr;
        PFNGLGETSHADERIVPROC GetShaderiv = nullptr;
        PFNGLGETSHADERINFOLOGPROC GetShaderInfoLog = nullptr;
        PFNGLDELETESHADERPROC DeleteShader = nullptr;

        PFNGLCREATEPROGRAMPROC CreateProgram = nullptr;
        PFNGLATTACHSHADERPROC AttachShader = nullptr;
        PFNGLDETACHSHADERPROC DetachShader = nullptr;
        PFNGLLINKPROGRAMPROC LinkProgram = nullptr;
        PFNGLGETPROGRAMIVPROC GetProgramiv = nullptr;
        PFNGLGETPROGRAMINFOLOGPROC GetProgramInfoLog = nullptr;
        PFNGLUSEPROGRAMPROC UseProgram = nullptr;
        PFNGLDELETEPROGRAMPROC DeleteProgram = nullptr;

        PFNGLGETUNIFORMLOCATIONPROC GetUniformLocation = nullptr;
        PFNGLUNIFORM1IPROC Uniform1i = nullptr;
        PFNGLUNIFORM1FPROC Uniform1f = nullptr;
        PFNGLUNIFORM3FPROC Uniform3f = nullptr;
        PFNGLUNIFORM4FPROC Uniform4f = nullptr;
        PFNGLUNIFORMMATRIX4FVPROC UniformMatrix4fv = nullptr;

        PFNGLGENVERTEXARRAYSPROC GenVertexArrays = nullptr;
        PFNGLDELETEVERTEXARRAYSPROC DeleteVertexArrays = nullptr;
        PFNGLBINDVERTEXARRAYPROC BindVertexArray = nullptr;
        PFNGLENABLEATTRIBARRAYPROC EnableVertexAttribArray = nullptr;
        PFNGLVERTEXATTRIBPOINTERPROC VertexAttribPointer = nullptr;

        PFNGLCLEARPROC Clear = nullptr;
        PFNGLCLEARCOLORPROC ClearColor = nullptr;
        PFNGLVIEWPORTPROC Viewport = nullptr;
        PFNGLDRAWARRAYSPROC DrawArrays = nullptr;
        PFNGLDRAWELEMENTSPROC DrawElements = nullptr;

        PFNGLGENTEXTURESPROC GenTextures = nullptr;
        PFNGLDELETETEXTURESPROC DeleteTextures = nullptr;
        PFNGLBINDTEXTUREPROC BindTexture = nullptr;
        PFNGLTEXIMAGE2DPROC TexImage2D = nullptr;
        PFNGLTEXPARAMETERIPROC TexParameteri = nullptr;

        PFNGLENABLEPROC Enable = nullptr;
        PFNGLDISABLEPROC Disable = nullptr;
        PFNGLBLENDFUNCPROC BlendFunc = nullptr;
    };

    bool initialize_gl_functions(GlFunctions& functions, GLGetProcAddressFn callback);

    // Global instance used by the library internally
    extern GlFunctions g_gl;
}