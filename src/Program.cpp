#include "easygl/Program.hpp"
#include "easygl/Exception.hpp"
#include "easygl/Shader.hpp"
#include "platform/GlFunctions.hpp"
#include <algorithm>
#include <utility>
#include <vector>

namespace easygl
{
    Program::Program() = default;

    Program::Program(const std::string& vertex_source, const std::string& fragment_source)
    {
        compile_from_sources(vertex_source, fragment_source);
    }

    Program::~Program()
    {
        destroy();
    }

    Program::Program(Program&& other) noexcept
        : handle_(other.handle_)
        , linked_(other.linked_)
        , owned_shader_handles_(std::move(other.owned_shader_handles_))
    {
        other.handle_ = 0;
        other.linked_ = false;
        other.owned_shader_handles_.clear();
    }

    Program& Program::operator=(Program&& other) noexcept
    {
        if (this != &other)
        {
            destroy();
            handle_ = other.handle_;
            linked_ = other.linked_;
            owned_shader_handles_ = std::move(other.owned_shader_handles_);
            other.handle_ = 0;
            other.linked_ = false;
            other.owned_shader_handles_.clear();
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
            for (const auto shader_handle : owned_shader_handles_)
            {
                platform::g_gl.DetachShader(handle_, shader_handle);
                platform::g_gl.DeleteShader(shader_handle);
            }
        }
        else
        {
            for (const auto shader_handle : owned_shader_handles_)
            {
                platform::g_gl.DeleteShader(shader_handle);
            }
        }
        owned_shader_handles_.clear();

        if (handle_ != 0)
        {
            platform::g_gl.DeleteProgram(handle_);
            handle_ = 0;
        }

        linked_ = false;
    }

    void Program::attach(const Shader& shader)
    {
        if (!is_created()) create();
        platform::g_gl.AttachShader(handle_, shader.native_handle());
    }

    void Program::attach_owned(Shader& shader)
    {
        attach(shader);
        const auto shader_handle = shader.release_native_handle();
        if (shader_handle == 0)
        {
            return;
        }

        if (std::find(owned_shader_handles_.begin(), owned_shader_handles_.end(), shader_handle) == owned_shader_handles_.end())
        {
            owned_shader_handles_.push_back(shader_handle);
        }
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

        if (linked_)
        {
            for (const auto shader_handle : owned_shader_handles_)
            {
                platform::g_gl.DetachShader(handle_, shader_handle);
                platform::g_gl.DeleteShader(shader_handle);
            }
            owned_shader_handles_.clear();
        }
    }

    void Program::compile_from_sources(const std::string& vertex_source, const std::string& fragment_source)
    {
        destroy();

        Shader vertex_shader(ShaderStage::Vertex);
        vertex_shader.compile_from_source(vertex_source);
        if (!vertex_shader.is_compiled())
        {
            throw Exception("Vertex shader compilation failed:\n" + vertex_shader.info_log());
        }

        Shader fragment_shader(ShaderStage::Fragment);
        fragment_shader.compile_from_source(fragment_source);
        if (!fragment_shader.is_compiled())
        {
            throw Exception("Fragment shader compilation failed:\n" + fragment_shader.info_log());
        }

        attach_owned(vertex_shader);
        attach_owned(fragment_shader);
        link();

        if (!is_linked())
        {
            const auto log = info_log();
            destroy();
            throw Exception("Program linking failed:\n" + log);
        }
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

    void Program::set_uniform(int location, float x, float y, float z, float w)
    {
        platform::g_gl.Uniform4f(location, x, y, z, w);
    }

    void Program::set_uniform_matrix4(int location, const float* data, bool transpose)
    {
        platform::g_gl.UniformMatrix4fv(location, 1, transpose ? 1 : 0, data);
    }

    bool Program::is_linked() const noexcept { return linked_; }
    bool Program::is_created() const noexcept { return handle_ != 0; }
    unsigned int Program::native_handle() const noexcept { return handle_; }
}
