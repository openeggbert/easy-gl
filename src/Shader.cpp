#include "easygl/Shader.hpp"

namespace easygl
{
    Shader::Shader(ShaderStage stage) : stage_(stage) {}
    Shader::~Shader()
    {
        destroy();
    }

    Shader::Shader(Shader&& other) noexcept
        : stage_(other.stage_)
        , handle_(other.handle_)
        , compiled_(other.compiled_)
    {
        other.handle_ = 0;
        other.compiled_ = false;
    }

    Shader& Shader::operator=(Shader&& other) noexcept
    {
        if (this != &other)
        {
            destroy();
            stage_ = other.stage_;
            handle_ = other.handle_;
            compiled_ = other.compiled_;
            other.handle_ = 0;
            other.compiled_ = false;
        }
        return *this;
    }

    void Shader::create()
    {
        // TODO: glCreateShader(stage_to_gl(stage_))
    }

    void Shader::destroy() noexcept
    {
        if (handle_ != 0)
        {
            // TODO: glDeleteShader(handle_)
            handle_ = 0;
            compiled_ = false;
        }
    }

    void Shader::compile_from_source(const std::string&)
    {
        // TODO: Implement shader compilation
    }

    std::string Shader::info_log() const
    {
        // TODO: glGetShaderInfoLog
        return "";
    }

    ShaderStage Shader::stage() const noexcept { return stage_; }
    bool Shader::is_compiled() const noexcept { return compiled_; }
    bool Shader::is_created() const noexcept { return handle_ != 0; }
    unsigned int Shader::native_handle() const noexcept { return handle_; }
}
