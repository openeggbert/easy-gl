#include "easygl/Program.hpp"
#include "easygl/Exception.hpp"
#include "easygl/Shader.hpp"
#include <metagl/metagl.hpp>
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
        , generation_(other.generation_)
    {
        other.handle_ = 0;
        other.linked_ = false;
        other.owned_shader_handles_.clear();
        other.generation_ = 0;
    }

    Program& Program::operator=(Program&& other) noexcept
    {
        if (this != &other)
        {
            destroy();
            handle_ = other.handle_;
            linked_ = other.linked_;
            owned_shader_handles_ = std::move(other.owned_shader_handles_);
            generation_ = other.generation_;
            other.handle_ = 0;
            other.linked_ = false;
            other.owned_shader_handles_.clear();
            other.generation_ = 0;
        }
        return *this;
    }

    void Program::create()
    {
        if (is_created()) return;
        handle_ = metagl::glCreateProgram();
        generation_ = metagl::GetContextGeneration();
    }

    void Program::destroy() noexcept
    {
        if (handle_ != 0)
        {
            for (const auto shader_handle : owned_shader_handles_)
            {
                metagl::glDetachShader(handle_, shader_handle);
                metagl::glDeleteShader(shader_handle);
            }
        }
        else
        {
            for (const auto shader_handle : owned_shader_handles_)
                metagl::glDeleteShader(shader_handle);
        }
        owned_shader_handles_.clear();

        if (handle_ != 0)
        {
            metagl::glDeleteProgram(handle_);
            handle_ = 0;
        }

        linked_ = false;
    }

    void Program::attach(const Shader& shader)
    {
        if (!is_created()) create();
        metagl::glAttachShader(handle_, shader.native_handle());
    }

    void Program::attach_owned(Shader& shader)
    {
        attach(shader);
        const auto shader_handle = shader.release_native_handle();
        if (shader_handle == 0) return;

        if (std::find(owned_shader_handles_.begin(), owned_shader_handles_.end(), shader_handle) == owned_shader_handles_.end())
            owned_shader_handles_.push_back(shader_handle);
    }

    void Program::detach(const Shader& shader)
    {
        if (is_created())
            metagl::glDetachShader(handle_, shader.native_handle());
    }

    void Program::link()
    {
        if (!is_created()) return;
        metagl::glLinkProgram(handle_);

        int status = 0;
        metagl::glGetProgramiv(handle_, metagl::ProgramParameter::LinkStatus, &status);
        linked_ = (status != 0);

        if (linked_)
        {
            for (const auto shader_handle : owned_shader_handles_)
            {
                metagl::glDetachShader(handle_, shader_handle);
                metagl::glDeleteShader(shader_handle);
            }
            owned_shader_handles_.clear();
        }
    }

    void Program::compile_from_sources(const std::string& vertex_source, const std::string& fragment_source)
    {
        destroy();

        Shader vertex_shader(metagl::ShaderType::Vertex);
        vertex_shader.compile_from_source(vertex_source);
        if (!vertex_shader.is_compiled())
            throw Exception("Vertex shader compilation failed:\n" + vertex_shader.info_log());

        Shader fragment_shader(metagl::ShaderType::Fragment);
        fragment_shader.compile_from_source(fragment_source);
        if (!fragment_shader.is_compiled())
            throw Exception("Fragment shader compilation failed:\n" + fragment_shader.info_log());

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
        metagl::glUseProgram(handle_);
    }

    void Program::validate() const
    {
        if (is_created())
            metagl::glValidateProgram(handle_);
    }

    std::string Program::info_log() const
    {
        if (!is_created()) return "";

        int length = 0;
        metagl::glGetProgramiv(handle_, metagl::ProgramParameter::InfoLogLength, &length);
        if (length <= 0) return "";

        std::vector<char> buffer(static_cast<std::size_t>(length));
        metagl::glGetProgramInfoLog(handle_, length, nullptr, buffer.data());
        return std::string(buffer.data());
    }

    int Program::attrib_location(const std::string& name) const
    {
        if (!is_created()) return -1;
        return metagl::glGetAttribLocation(handle_, name.c_str());
    }

    void Program::bind_attrib_location(unsigned int index, const std::string& name)
    {
        if (!is_created()) create();
        metagl::glBindAttribLocation(handle_, index, name.c_str());
    }

    int Program::uniform_location(const std::string& name) const
    {
        if (!is_created()) return -1;
        return metagl::glGetUniformLocation(handle_, name.c_str());
    }

    // float
    void Program::set_uniform(int location, float value)               { metagl::glUniform1f(location, value); }
    void Program::set_uniform(int location, float x, float y)          { metagl::glUniform2f(location, x, y); }
    void Program::set_uniform(int location, float x, float y, float z) { metagl::glUniform3f(location, x, y, z); }
    void Program::set_uniform(int location, float x, float y, float z, float w) { metagl::glUniform4f(location, x, y, z, w); }

    void Program::set_uniform_fv(int location, std::span<const float> values, int components)
    {
        const auto count = static_cast<GLsizei>(values.size() / static_cast<std::size_t>(components));
        switch (components)
        {
            case 1: metagl::glUniform1fv(location, count, values.data()); break;
            case 2: metagl::glUniform2fv(location, count, values.data()); break;
            case 3: metagl::glUniform3fv(location, count, values.data()); break;
            case 4: metagl::glUniform4fv(location, count, values.data()); break;
            default: break;
        }
    }

    // int
    void Program::set_uniform(int location, int value)                { metagl::glUniform1i(location, value); }
    void Program::set_uniform(int location, int x, int y)             { metagl::glUniform2i(location, x, y); }
    void Program::set_uniform(int location, int x, int y, int z)      { metagl::glUniform3i(location, x, y, z); }
    void Program::set_uniform(int location, int x, int y, int z, int w) { metagl::glUniform4i(location, x, y, z, w); }

    // unsigned int
    void Program::set_uniform(int location, unsigned int value)                        { metagl::glUniform1ui(location, value); }
    void Program::set_uniform(int location, unsigned int x, unsigned int y)             { metagl::glUniform2ui(location, x, y); }
    void Program::set_uniform(int location, unsigned int x, unsigned int y, unsigned int z) { metagl::glUniform3ui(location, x, y, z); }
    void Program::set_uniform(int location, unsigned int x, unsigned int y, unsigned int z, unsigned int w) { metagl::glUniform4ui(location, x, y, z, w); }

    // matrices
    void Program::set_uniform_matrix2(int location, const float* data, bool transpose)
    {
        metagl::glUniformMatrix2fv(location, 1, transpose ? 1 : 0, data);
    }
    void Program::set_uniform_matrix3(int location, const float* data, bool transpose)
    {
        metagl::glUniformMatrix3fv(location, 1, transpose ? 1 : 0, data);
    }
    void Program::set_uniform_matrix4(int location, const float* data, bool transpose)
    {
        metagl::glUniformMatrix4fv(location, 1, transpose ? 1 : 0, data);
    }
    void Program::set_uniform_matrix2x3(int location, const float* data, bool transpose)
    {
        metagl::glUniformMatrix2x3fv(location, 1, transpose ? 1 : 0, data);
    }
    void Program::set_uniform_matrix3x2(int location, const float* data, bool transpose)
    {
        metagl::glUniformMatrix3x2fv(location, 1, transpose ? 1 : 0, data);
    }
    void Program::set_uniform_matrix2x4(int location, const float* data, bool transpose)
    {
        metagl::glUniformMatrix2x4fv(location, 1, transpose ? 1 : 0, data);
    }
    void Program::set_uniform_matrix4x2(int location, const float* data, bool transpose)
    {
        metagl::glUniformMatrix4x2fv(location, 1, transpose ? 1 : 0, data);
    }
    void Program::set_uniform_matrix3x4(int location, const float* data, bool transpose)
    {
        metagl::glUniformMatrix3x4fv(location, 1, transpose ? 1 : 0, data);
    }
    void Program::set_uniform_matrix4x3(int location, const float* data, bool transpose)
    {
        metagl::glUniformMatrix4x3fv(location, 1, transpose ? 1 : 0, data);
    }

    unsigned int Program::uniform_block_index(const std::string& name) const
    {
        if (!is_created()) return GL_INVALID_INDEX;
        return metagl::glGetUniformBlockIndex(handle_, name.c_str());
    }

    void Program::set_uniform_block_binding(unsigned int block_index, unsigned int binding_point)
    {
        metagl::glUniformBlockBinding(handle_, block_index, binding_point);
    }

    bool Program::is_linked() const noexcept { return linked_; }
    bool Program::is_created() const noexcept { return handle_ != 0; }

    void Program::reset_handle_no_gl() noexcept
    {
        handle_ = 0;
        generation_ = 0;
        linked_ = false;
        owned_shader_handles_.clear();
    }

    unsigned int Program::native_handle() const noexcept { return handle_; }

    bool Program::is_valid_for_current_generation() const noexcept
    {
        return handle_ != 0 && generation_ == metagl::GetContextGeneration();
    }

    std::uint64_t Program::creation_generation() const noexcept { return generation_; }
}
