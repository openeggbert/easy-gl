#include "easygl/Shader.hpp"
#include <metagl/metagl.hpp>
#include <vector>

namespace easygl
{
    static metagl::ShaderType to_meta(ShaderStage stage)
    {
        switch (stage)
        {
            case ShaderStage::Vertex:         return metagl::ShaderType::Vertex;
            case ShaderStage::Fragment:       return metagl::ShaderType::Fragment;
            case ShaderStage::Geometry:       return metagl::ShaderType::Geometry;
            case ShaderStage::TessControl:    return metagl::ShaderType::TessControl;
            case ShaderStage::TessEvaluation: return metagl::ShaderType::TessEvaluation;
            case ShaderStage::Compute:        return metagl::ShaderType::Compute;
            default:                          return metagl::ShaderType::Vertex;
        }
    }

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
        if (is_created()) return;
        handle_ = metagl::glCreateShader(to_meta(stage_));
    }

    void Shader::destroy() noexcept
    {
        if (handle_ != 0)
        {
            metagl::glDeleteShader(handle_);
            handle_ = 0;
            compiled_ = false;
        }
    }

    void Shader::compile_from_source(const std::string& source)
    {
        if (!is_created()) create();

        const char* src = source.c_str();
        metagl::glShaderSource(handle_, 1, &src, nullptr);
        metagl::glCompileShader(handle_);

        metagl::GLint status = 0;
        metagl::glGetShaderiv(handle_, metagl::ShaderParameter::CompileStatus, &status);
        compiled_ = (status != 0);
    }

    unsigned int Shader::release_native_handle() noexcept
    {
        const unsigned int released = handle_;
        handle_ = 0;
        compiled_ = false;
        return released;
    }

    std::string Shader::info_log() const
    {
        if (!is_created()) return "";

        metagl::GLint length = 0;
        metagl::glGetShaderiv(handle_, metagl::ShaderParameter::InfoLogLength, &length);
        if (length <= 0) return "";

        std::vector<char> buffer(static_cast<std::size_t>(length));
        metagl::glGetShaderInfoLog(handle_, length, nullptr, buffer.data());
        return std::string(buffer.data());
    }

    ShaderStage Shader::stage() const noexcept { return stage_; }
    bool Shader::is_compiled() const noexcept { return compiled_; }
    bool Shader::is_created() const noexcept { return handle_ != 0; }
    unsigned int Shader::native_handle() const noexcept { return handle_; }
}
