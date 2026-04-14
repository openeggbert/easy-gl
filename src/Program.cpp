#include "easygl/Program.hpp"
#include "easygl/Shader.hpp"

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
        // TODO: glCreateProgram
    }

    void Program::destroy() noexcept
    {
        if (handle_ != 0)
        {
            // TODO: glDeleteProgram(handle_)
            handle_ = 0;
            linked_ = false;
        }
    }

    void Program::attach(const Shader&)
    {
        // TODO: glAttachShader
    }

    void Program::detach(const Shader&)
    {
        // TODO: glDetachShader
    }

    void Program::link()
    {
        // TODO: glLinkProgram
    }

    void Program::use() const
    {
        // TODO: glUseProgram
    }

    std::string Program::info_log() const
    {
        // TODO: glGetProgramInfoLog
        return "";
    }

    int Program::uniform_location(const std::string&) const
    {
        // TODO: glGetUniformLocation
        return -1;
    }

    void Program::set_uniform(int, int)
    {
        // TODO: glUniform1i
    }

    void Program::set_uniform(int, float)
    {
        // TODO: glUniform1f
    }

    void Program::set_uniform(int, float, float, float)
    {
        // TODO: glUniform3f
    }

    void Program::set_uniform_matrix4(int, const float*, bool)
    {
        // TODO: glUniformMatrix4fv
    }

    bool Program::is_linked() const noexcept { return linked_; }
    bool Program::is_created() const noexcept { return handle_ != 0; }
    unsigned int Program::native_handle() const noexcept { return handle_; }
}
