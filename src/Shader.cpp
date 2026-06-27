#include "easygl/Shader.hpp"
#include <metagl/metagl.hpp>
#include <vector>

namespace easygl
{
    Shader::Shader(ShaderType type) : type_(type) {}
    Shader::~Shader()
    {
        destroy();
    }

    Shader::Shader(Shader&& other) noexcept
        : type_(other.type_)
        , compiled_(other.compiled_)
    {
        handle_ = other.handle_;
        generation_ = other.generation_;
        other.handle_ = 0;
        other.compiled_ = false;
        other.generation_ = 0;
    }

    Shader& Shader::operator=(Shader&& other) noexcept
    {
        if (this != &other)
        {
            destroy();
            type_ = other.type_;
            handle_ = other.handle_;
            compiled_ = other.compiled_;
            generation_ = other.generation_;
            other.handle_ = 0;
            other.compiled_ = false;
            other.generation_ = 0;
        }
        return *this;
    }

    void Shader::create()
    {
        if (is_created()) return;
        metagl::ShaderId sid = metagl::glCreateShader(type_);
        handle_ = sid.value;
        generation_ = metagl::GetContextGeneration();
    }

    void Shader::destroy() noexcept
    {
        if (handle_ != 0)
        {
            metagl::glDeleteShader(metagl::ShaderId{handle_});
            handle_ = 0;
            compiled_ = false;
        }
    }

    void Shader::compile_from_source(const std::string& source)
    {
        if (!is_created()) create();

        const char* src = source.c_str();
        metagl::glShaderSource(metagl::ShaderId{handle_}, 1, &src, nullptr);
        metagl::glCompileShader(metagl::ShaderId{handle_});

        int status = 0;
        metagl::glGetShaderiv(metagl::ShaderId{handle_}, metagl::ShaderParameter::CompileStatus, &status);
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

        int length = 0;
        metagl::glGetShaderiv(metagl::ShaderId{handle_}, metagl::ShaderParameter::InfoLogLength, &length);
        if (length <= 0) return "";

        std::vector<char> buffer(static_cast<std::size_t>(length));
        metagl::glGetShaderInfoLog(metagl::ShaderId{handle_}, length, nullptr, buffer.data());
        return std::string(buffer.data());
    }

    ShaderType Shader::shader_type() const noexcept { return type_; }
    bool Shader::is_compiled() const noexcept { return compiled_; }
}
namespace easygl { bool Shader::is_valid_gl_object() const { return is_created() && metagl::glIsShader(metagl::ShaderId{handle_}); } }
