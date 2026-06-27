#include "easygl/Program.hpp"
#include "easygl/Exception.hpp"
#include "easygl/Shader.hpp"
#include <metagl/metagl.hpp>
#include <algorithm>
#include <optional>
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
        : linked_(other.linked_)
        , owned_shader_handles_(std::move(other.owned_shader_handles_))
    {
        handle_ = other.handle_;
        generation_ = other.generation_;
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
        metagl::ProgramId pid = metagl::glCreateProgram();
        handle_ = pid.value;
        generation_ = metagl::GetContextGeneration();
    }

    void Program::destroy() noexcept
    {
        if (handle_ != 0)
        {
            for (const auto shader_handle : owned_shader_handles_)
            {
                metagl::glDetachShader(metagl::ProgramId{handle_}, metagl::ShaderId{shader_handle});
                metagl::glDeleteShader(metagl::ShaderId{shader_handle});
            }
        }
        else
        {
            for (const auto shader_handle : owned_shader_handles_)
                metagl::glDeleteShader(metagl::ShaderId{shader_handle});
        }
        owned_shader_handles_.clear();

        if (handle_ != 0)
        {
            metagl::glDeleteProgram(metagl::ProgramId{handle_});
            handle_ = 0;
        }

        linked_ = false;
    }

    void Program::attach(const Shader& shader)
    {
        if (!is_created()) create();
        metagl::glAttachShader(metagl::ProgramId{handle_}, metagl::ShaderId{shader.native_handle()});
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
            metagl::glDetachShader(metagl::ProgramId{handle_}, metagl::ShaderId{shader.native_handle()});
    }

    void Program::link()
    {
        if (!is_created()) return;
        metagl::glLinkProgram(metagl::ProgramId{handle_});

        int status = 0;
        metagl::glGetProgramiv(metagl::ProgramId{handle_}, metagl::ProgramParameter::LinkStatus, &status);
        linked_ = (status != 0);

        if (linked_)
        {
            for (const auto shader_handle : owned_shader_handles_)
            {
                metagl::glDetachShader(metagl::ProgramId{handle_}, metagl::ShaderId{shader_handle});
                metagl::glDeleteShader(metagl::ShaderId{shader_handle});
            }
            owned_shader_handles_.clear();
        }
    }

    Program Program::from_sources(const std::string& vertex_source, const std::string& fragment_source)
    {
        Program p;
        p.compile_from_sources(vertex_source, fragment_source);
        return p;
    }

    Program Program::from_sources(const std::string& vertex_source,
                                   const std::string& geometry_source,
                                   const std::string& fragment_source)
    {
        Program p;
        p.destroy();

        Shader vertex_shader(metagl::ShaderType::Vertex);
        vertex_shader.compile_from_source(vertex_source);
        if (!vertex_shader.is_compiled())
            throw Exception("Vertex shader compilation failed:\n" + vertex_shader.info_log());

        Shader geometry_shader(metagl::ShaderType::Geometry);
        geometry_shader.compile_from_source(geometry_source);
        if (!geometry_shader.is_compiled())
            throw Exception("Geometry shader compilation failed:\n" + geometry_shader.info_log());

        Shader fragment_shader(metagl::ShaderType::Fragment);
        fragment_shader.compile_from_source(fragment_source);
        if (!fragment_shader.is_compiled())
            throw Exception("Fragment shader compilation failed:\n" + fragment_shader.info_log());

        p.attach_owned(vertex_shader);
        p.attach_owned(geometry_shader);
        p.attach_owned(fragment_shader);
        p.link();

        if (!p.is_linked())
        {
            const auto log = p.info_log();
            p.destroy();
            throw Exception("Program linking failed:\n" + log);
        }
        return p;
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
        metagl::glUseProgram(metagl::ProgramId{handle_});
    }

    void Program::set_parameter(ProgramParameter pname, int value)
    {
        if (!is_created()) create();
        metagl::glProgramParameteri(metagl::ProgramId{handle_}, pname, value);
    }

    Program Program::create_separable(ShaderType type, const std::string& source)
    {
        Shader shader(type);
        shader.compile_from_source(source);
        if (!shader.is_compiled())
            throw Exception("Shader compilation failed:\n" + shader.info_log());

        Program p;
        p.create();
        p.set_parameter(metagl::ProgramParameter::Separable, GL_TRUE);
        p.attach_owned(shader);
        p.link();

        if (!p.is_linked())
        {
            const auto log = p.info_log();
            p.destroy();
            throw Exception("Separable program linking failed:\n" + log);
        }
        return p;
    }

    void Program::validate() const
    {
        if (is_created())
            metagl::glValidateProgram(metagl::ProgramId{handle_});
    }

    std::string Program::info_log() const
    {
        if (!is_created()) return "";

        int length = 0;
        metagl::glGetProgramiv(metagl::ProgramId{handle_}, metagl::ProgramParameter::InfoLogLength, &length);
        if (length <= 0) return "";

        std::vector<char> buffer(static_cast<std::size_t>(length));
        metagl::glGetProgramInfoLog(metagl::ProgramId{handle_}, length, nullptr, buffer.data());
        return std::string(buffer.data());
    }

    int Program::attrib_location(const std::string& name) const
    {
        if (!is_created()) return -1;
        return static_cast<int>(metagl::glGetAttribLocation(metagl::ProgramId{handle_}, name.c_str()).value);
    }

    void Program::bind_attrib_location(unsigned int index, const std::string& name)
    {
        if (!is_created()) create();
        metagl::glBindAttribLocation(metagl::ProgramId{handle_}, metagl::AttribLocation{index}, name.c_str());
    }

    int Program::uniform_location(const std::string& name) const
    {
        if (!is_created()) return -1;
        return metagl::glGetUniformLocation(metagl::ProgramId{handle_}, name.c_str()).value;
    }

    // float
    void Program::set_uniform(int location, float value)               { metagl::glUniform1f(metagl::UniformLocation{location}, value); }
    void Program::set_uniform(int location, float x, float y)          { metagl::glUniform2f(metagl::UniformLocation{location}, x, y); }
    void Program::set_uniform(int location, float x, float y, float z) { metagl::glUniform3f(metagl::UniformLocation{location}, x, y, z); }
    void Program::set_uniform(int location, float x, float y, float z, float w) { metagl::glUniform4f(metagl::UniformLocation{location}, x, y, z, w); }

    void Program::set_uniform_fv(int location, std::span<const float> values, int components)
    {
        const auto loc = metagl::UniformLocation{location};
        const auto count = static_cast<GLsizei>(values.size() / static_cast<std::size_t>(components));
        switch (components)
        {
            case 1: metagl::glUniform1fv(loc, count, values.data()); break;
            case 2: metagl::glUniform2fv(loc, count, values.data()); break;
            case 3: metagl::glUniform3fv(loc, count, values.data()); break;
            case 4: metagl::glUniform4fv(loc, count, values.data()); break;
            default: break;
        }
    }

    void Program::set_uniform_iv(int location, std::span<const int> values, int components)
    {
        const auto loc = metagl::UniformLocation{location};
        const auto count = static_cast<GLsizei>(values.size() / static_cast<std::size_t>(components));
        switch (components)
        {
            case 1: metagl::glUniform1iv(loc, count, values.data()); break;
            case 2: metagl::glUniform2iv(loc, count, values.data()); break;
            case 3: metagl::glUniform3iv(loc, count, values.data()); break;
            case 4: metagl::glUniform4iv(loc, count, values.data()); break;
            default: break;
        }
    }

    void Program::set_uniform_uiv(int location, std::span<const unsigned int> values, int components)
    {
        const auto loc = metagl::UniformLocation{location};
        const auto count = static_cast<GLsizei>(values.size() / static_cast<std::size_t>(components));
        switch (components)
        {
            case 1: metagl::glUniform1uiv(loc, count, values.data()); break;
            case 2: metagl::glUniform2uiv(loc, count, values.data()); break;
            case 3: metagl::glUniform3uiv(loc, count, values.data()); break;
            case 4: metagl::glUniform4uiv(loc, count, values.data()); break;
            default: break;
        }
    }

    // int
    void Program::set_uniform(int location, int value)                { metagl::glUniform1i(metagl::UniformLocation{location}, value); }
    void Program::set_uniform(int location, int x, int y)             { metagl::glUniform2i(metagl::UniformLocation{location}, x, y); }
    void Program::set_uniform(int location, int x, int y, int z)      { metagl::glUniform3i(metagl::UniformLocation{location}, x, y, z); }
    void Program::set_uniform(int location, int x, int y, int z, int w) { metagl::glUniform4i(metagl::UniformLocation{location}, x, y, z, w); }

    // unsigned int
    void Program::set_uniform(int location, unsigned int value)                        { metagl::glUniform1ui(metagl::UniformLocation{location}, value); }
    void Program::set_uniform(int location, unsigned int x, unsigned int y)             { metagl::glUniform2ui(metagl::UniformLocation{location}, x, y); }
    void Program::set_uniform(int location, unsigned int x, unsigned int y, unsigned int z) { metagl::glUniform3ui(metagl::UniformLocation{location}, x, y, z); }
    void Program::set_uniform(int location, unsigned int x, unsigned int y, unsigned int z, unsigned int w) { metagl::glUniform4ui(metagl::UniformLocation{location}, x, y, z, w); }

    // matrices
    void Program::set_uniform_matrix2(int location, const float* data, bool transpose)
    {
        metagl::glUniformMatrix2fv(metagl::UniformLocation{location}, 1, transpose ? 1 : 0, data);
    }
    void Program::set_uniform_matrix3(int location, const float* data, bool transpose)
    {
        metagl::glUniformMatrix3fv(metagl::UniformLocation{location}, 1, transpose ? 1 : 0, data);
    }
    void Program::set_uniform_matrix4(int location, const float* data, bool transpose)
    {
        metagl::glUniformMatrix4fv(metagl::UniformLocation{location}, 1, transpose ? 1 : 0, data);
    }
    void Program::set_uniform_matrix2x3(int location, const float* data, bool transpose)
    {
        metagl::glUniformMatrix2x3fv(metagl::UniformLocation{location}, 1, transpose ? 1 : 0, data);
    }
    void Program::set_uniform_matrix3x2(int location, const float* data, bool transpose)
    {
        metagl::glUniformMatrix3x2fv(metagl::UniformLocation{location}, 1, transpose ? 1 : 0, data);
    }
    void Program::set_uniform_matrix2x4(int location, const float* data, bool transpose)
    {
        metagl::glUniformMatrix2x4fv(metagl::UniformLocation{location}, 1, transpose ? 1 : 0, data);
    }
    void Program::set_uniform_matrix4x2(int location, const float* data, bool transpose)
    {
        metagl::glUniformMatrix4x2fv(metagl::UniformLocation{location}, 1, transpose ? 1 : 0, data);
    }
    void Program::set_uniform_matrix3x4(int location, const float* data, bool transpose)
    {
        metagl::glUniformMatrix3x4fv(metagl::UniformLocation{location}, 1, transpose ? 1 : 0, data);
    }
    void Program::set_uniform_matrix4x3(int location, const float* data, bool transpose)
    {
        metagl::glUniformMatrix4x3fv(metagl::UniformLocation{location}, 1, transpose ? 1 : 0, data);
    }

    // Separable program uniforms
    void Program::set_program_uniform(int location, float value) const               { metagl::glProgramUniform1f(metagl::ProgramId{handle_}, metagl::UniformLocation{location}, value); }
    void Program::set_program_uniform(int location, float x, float y) const          { metagl::glProgramUniform2f(metagl::ProgramId{handle_}, metagl::UniformLocation{location}, x, y); }
    void Program::set_program_uniform(int location, float x, float y, float z) const { metagl::glProgramUniform3f(metagl::ProgramId{handle_}, metagl::UniformLocation{location}, x, y, z); }
    void Program::set_program_uniform(int location, float x, float y, float z, float w) const { metagl::glProgramUniform4f(metagl::ProgramId{handle_}, metagl::UniformLocation{location}, x, y, z, w); }

    void Program::set_program_uniform_fv(int location, std::span<const float> values, int components) const
    {
        const auto pid = metagl::ProgramId{handle_};
        const auto loc = metagl::UniformLocation{location};
        const auto count = static_cast<GLsizei>(values.size() / static_cast<std::size_t>(components));
        switch (components)
        {
            case 1: metagl::glProgramUniform1fv(pid, loc, count, values.data()); break;
            case 2: metagl::glProgramUniform2fv(pid, loc, count, values.data()); break;
            case 3: metagl::glProgramUniform3fv(pid, loc, count, values.data()); break;
            case 4: metagl::glProgramUniform4fv(pid, loc, count, values.data()); break;
            default: break;
        }
    }

    void Program::set_program_uniform(int location, int value) const                { metagl::glProgramUniform1i(metagl::ProgramId{handle_}, metagl::UniformLocation{location}, value); }
    void Program::set_program_uniform(int location, int x, int y) const             { metagl::glProgramUniform2i(metagl::ProgramId{handle_}, metagl::UniformLocation{location}, x, y); }
    void Program::set_program_uniform(int location, int x, int y, int z) const      { metagl::glProgramUniform3i(metagl::ProgramId{handle_}, metagl::UniformLocation{location}, x, y, z); }
    void Program::set_program_uniform(int location, int x, int y, int z, int w) const { metagl::glProgramUniform4i(metagl::ProgramId{handle_}, metagl::UniformLocation{location}, x, y, z, w); }

    void Program::set_program_uniform_iv(int location, std::span<const int> values, int components) const
    {
        const auto pid = metagl::ProgramId{handle_};
        const auto loc = metagl::UniformLocation{location};
        const auto count = static_cast<GLsizei>(values.size() / static_cast<std::size_t>(components));
        switch (components)
        {
            case 1: metagl::glProgramUniform1iv(pid, loc, count, values.data()); break;
            case 2: metagl::glProgramUniform2iv(pid, loc, count, values.data()); break;
            case 3: metagl::glProgramUniform3iv(pid, loc, count, values.data()); break;
            case 4: metagl::glProgramUniform4iv(pid, loc, count, values.data()); break;
            default: break;
        }
    }

    void Program::set_program_uniform(int location, unsigned int value) const                        { metagl::glProgramUniform1ui(metagl::ProgramId{handle_}, metagl::UniformLocation{location}, value); }
    void Program::set_program_uniform(int location, unsigned int x, unsigned int y) const             { metagl::glProgramUniform2ui(metagl::ProgramId{handle_}, metagl::UniformLocation{location}, x, y); }
    void Program::set_program_uniform(int location, unsigned int x, unsigned int y, unsigned int z) const { metagl::glProgramUniform3ui(metagl::ProgramId{handle_}, metagl::UniformLocation{location}, x, y, z); }
    void Program::set_program_uniform(int location, unsigned int x, unsigned int y, unsigned int z, unsigned int w) const { metagl::glProgramUniform4ui(metagl::ProgramId{handle_}, metagl::UniformLocation{location}, x, y, z, w); }

    void Program::set_program_uniform_uiv(int location, std::span<const unsigned int> values, int components) const
    {
        const auto pid = metagl::ProgramId{handle_};
        const auto loc = metagl::UniformLocation{location};
        const auto count = static_cast<GLsizei>(values.size() / static_cast<std::size_t>(components));
        switch (components)
        {
            case 1: metagl::glProgramUniform1uiv(pid, loc, count, values.data()); break;
            case 2: metagl::glProgramUniform2uiv(pid, loc, count, values.data()); break;
            case 3: metagl::glProgramUniform3uiv(pid, loc, count, values.data()); break;
            case 4: metagl::glProgramUniform4uiv(pid, loc, count, values.data()); break;
            default: break;
        }
    }

    void Program::set_program_uniform_matrix2(int location, const float* data, bool transpose) const
        { metagl::glProgramUniformMatrix2fv(metagl::ProgramId{handle_}, metagl::UniformLocation{location}, 1, transpose ? 1 : 0, data); }
    void Program::set_program_uniform_matrix3(int location, const float* data, bool transpose) const
        { metagl::glProgramUniformMatrix3fv(metagl::ProgramId{handle_}, metagl::UniformLocation{location}, 1, transpose ? 1 : 0, data); }
    void Program::set_program_uniform_matrix4(int location, const float* data, bool transpose) const
        { metagl::glProgramUniformMatrix4fv(metagl::ProgramId{handle_}, metagl::UniformLocation{location}, 1, transpose ? 1 : 0, data); }
    void Program::set_program_uniform_matrix2x3(int location, const float* data, bool transpose) const
        { metagl::glProgramUniformMatrix2x3fv(metagl::ProgramId{handle_}, metagl::UniformLocation{location}, 1, transpose ? 1 : 0, data); }
    void Program::set_program_uniform_matrix3x2(int location, const float* data, bool transpose) const
        { metagl::glProgramUniformMatrix3x2fv(metagl::ProgramId{handle_}, metagl::UniformLocation{location}, 1, transpose ? 1 : 0, data); }
    void Program::set_program_uniform_matrix2x4(int location, const float* data, bool transpose) const
        { metagl::glProgramUniformMatrix2x4fv(metagl::ProgramId{handle_}, metagl::UniformLocation{location}, 1, transpose ? 1 : 0, data); }
    void Program::set_program_uniform_matrix4x2(int location, const float* data, bool transpose) const
        { metagl::glProgramUniformMatrix4x2fv(metagl::ProgramId{handle_}, metagl::UniformLocation{location}, 1, transpose ? 1 : 0, data); }
    void Program::set_program_uniform_matrix3x4(int location, const float* data, bool transpose) const
        { metagl::glProgramUniformMatrix3x4fv(metagl::ProgramId{handle_}, metagl::UniformLocation{location}, 1, transpose ? 1 : 0, data); }
    void Program::set_program_uniform_matrix4x3(int location, const float* data, bool transpose) const
        { metagl::glProgramUniformMatrix4x3fv(metagl::ProgramId{handle_}, metagl::UniformLocation{location}, 1, transpose ? 1 : 0, data); }

    void Program::get_uniform_fv(int location, float* out) const
    {
        metagl::glGetUniformfv(metagl::ProgramId{handle_}, metagl::UniformLocation{location}, out);
    }

    void Program::get_uniform_iv(int location, int* out) const
    {
        metagl::glGetUniformiv(metagl::ProgramId{handle_}, metagl::UniformLocation{location}, out);
    }

    void Program::get_uniform_uiv(int location, unsigned int* out) const
    {
        metagl::glGetUniformuiv(metagl::ProgramId{handle_}, metagl::UniformLocation{location}, out);
    }

    std::optional<unsigned int> Program::uniform_block_index(const std::string& name) const
    {
        if (!is_created()) return std::nullopt;
        const unsigned int idx = metagl::glGetUniformBlockIndex(metagl::ProgramId{handle_}, name.c_str());
        if (idx == GL_INVALID_INDEX) return std::nullopt;
        return idx;
    }

    void Program::set_uniform_block_binding(unsigned int block_index, unsigned int binding_point)
    {
        metagl::glUniformBlockBinding(metagl::ProgramId{handle_}, block_index, binding_point);
    }

    bool Program::is_linked() const noexcept { return linked_; }

    void Program::reset_handle_no_gl() noexcept
    {
        handle_ = 0;
        generation_ = 0;
        linked_ = false;
        owned_shader_handles_.clear();
    }
}
namespace easygl { bool Program::is_valid_gl_object() const { return is_created() && metagl::glIsProgram(metagl::ProgramId{handle_}); } }
