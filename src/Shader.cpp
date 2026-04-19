#include "easygl/Shader.hpp"
#include "platform/GlFunctions.hpp"
#include <vector>

namespace easygl
{
    static platform::GLenum to_gl(ShaderStage stage)
    {
        switch (stage)
        {
            case ShaderStage::Vertex:         return platform::GL_VERTEX_SHADER;
            case ShaderStage::Fragment:       return platform::GL_FRAGMENT_SHADER;
            case ShaderStage::Geometry:       return platform::GL_GEOMETRY_SHADER;
            case ShaderStage::TessControl:    return platform::GL_TESS_CONTROL_SHADER;
            case ShaderStage::TessEvaluation: return platform::GL_TESS_EVALUATION_SHADER;
            case ShaderStage::Compute:        return platform::GL_COMPUTE_SHADER;
            default: return 0;
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
        handle_ = platform::g_gl.CreateShader(to_gl(stage_));
    }

    void Shader::destroy() noexcept
    {
        if (handle_ != 0)
        {
            platform::g_gl.DeleteShader(handle_);
            handle_ = 0;
            compiled_ = false;
        }
    }

    void Shader::compile_from_source(const std::string& source)
    {
        if (!is_created()) create();

        const char* src = source.c_str();
        platform::g_gl.ShaderSource(handle_, 1, &src, nullptr);
        platform::g_gl.CompileShader(handle_);

        platform::GLint status = 0;
        platform::g_gl.GetShaderiv(handle_, platform::GL_COMPILE_STATUS, &status);
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

        platform::GLint length = 0;
        platform::g_gl.GetShaderiv(handle_, platform::GL_INFO_LOG_LENGTH, &length);
        if (length <= 0) return "";

        std::vector<char> buffer(static_cast<std::size_t>(length));
        platform::g_gl.GetShaderInfoLog(handle_, length, nullptr, buffer.data());
        return std::string(buffer.data());
    }

    ShaderStage Shader::stage() const noexcept { return stage_; }
    bool Shader::is_compiled() const noexcept { return compiled_; }
    bool Shader::is_created() const noexcept { return handle_ != 0; }
    unsigned int Shader::native_handle() const noexcept { return handle_; }
}
