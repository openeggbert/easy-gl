#include "easygl/Program.hpp"
#include "easygl/Shader.hpp"
#include "platform/GlFunctions.hpp"
#include <vector>

namespace easygl
{
    Program::Program() = default;
    Program::~Program()
    {
        destroy();
    }

    Program::Program(Program&& other) noexcept
        : handle_(other.handle_)
        , linked_(other.linked_)
    {
        other.handle_ = 0;
        other.linked_ = false;
    }

    Program& Program::operator=(Program&& other) noexcept
    {
        if (this != &other)
        {
            destroy();
            handle_ = other.handle_;
            linked_ = other.linked_;
            other.handle_ = 0;
            other.linked_ = false;
        }
        return *this;
    }

    void Program::create()
    {
        if (is_created()) return;
        handle_ = platform::g_gl.CreateProgram();
    }

    void Program::destroy() noexcept
    {
        if (handle_ != 0)
        {
            platform::g_gl.DeleteProgram(handle_);
            handle_ = 0;
            linked_ = false;
        }
    }

    void Program::attach(const Shader& shader)
    {
        if (!is_created()) create();
        platform::g_gl.AttachShader(handle_, shader.native_handle());
    }

    void Program::detach(const Shader& shader)
    {
        if (is_created())
        {
            platform::g_gl.DetachShader(handle_, shader.native_handle());
        }
    }

    void Program::link()
    {
        if (!is_created()) return;
        platform::g_gl.LinkProgram(handle_);

        platform::GLint status = 0;
        platform::g_gl.GetProgramiv(handle_, platform::GL_LINK_STATUS, &status);
        linked_ = (status != 0);
    }

    void Program::use() const
    {
        platform::g_gl.UseProgram(handle_);
    }

    std::string Program::info_log() const
    {
        if (!is_created()) return "";

        platform::GLint length = 0;
        platform::g_gl.GetProgramiv(handle_, platform::GL_INFO_LOG_LENGTH, &length);
        if (length <= 0) return "";

        std::vector<char> buffer(static_cast<std::size_t>(length));
        platform::g_gl.GetProgramInfoLog(handle_, length, nullptr, buffer.data());
        return std::string(buffer.data());
    }

    int Program::uniform_location(const std::string& name) const
    {
        if (!is_created()) return -1;
        return platform::g_gl.GetUniformLocation(handle_, name.c_str());
    }

    void Program::set_uniform(int location, int value)
    {
        platform::g_gl.Uniform1i(location, value);
    }

    void Program::set_uniform(int location, float value)
    {
        platform::g_gl.Uniform1f(location, value);
    }

    void Program::set_uniform(int location, float x, float y, float z)
    {
        platform::g_gl.Uniform3f(location, x, y, z);
    }

    void Program::set_uniform_matrix4(int location, const float* data, bool transpose)
    {
        platform::g_gl.UniformMatrix4fv(location, 1, transpose ? 1 : 0, data);
    }

    bool Program::is_linked() const noexcept { return linked_; }
    bool Program::is_created() const noexcept { return handle_ != 0; }
    unsigned int Program::native_handle() const noexcept { return handle_; }
}
