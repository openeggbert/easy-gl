#pragma once
#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <vector>
#include "easygl/Export.hpp"
#include "easygl/Types.hpp"
#include "easygl/detail/NonCopyable.hpp"

namespace easygl
{
    class Shader;

    class EASYGL_API Program : public detail::NonCopyable
    {
    public:
        Program();
        Program(const std::string& vertex_source, const std::string& fragment_source);
        ~Program();

        Program(Program&& other) noexcept;
        Program& operator=(Program&& other) noexcept;

        void create();
        void destroy() noexcept;
        void attach(const Shader& shader);
        void attach_owned(Shader& shader);
        void detach(const Shader& shader);
        void link();
        void compile_from_sources(const std::string& vertex_source, const std::string& fragment_source);
        void use() const;
        void validate() const;

        [[nodiscard]] std::string info_log() const;

        // Attribute locations
        [[nodiscard]] int attrib_location(const std::string& name) const;
        void bind_attrib_location(unsigned int index, const std::string& name);

        // Uniforms — location lookup
        [[nodiscard]] int uniform_location(const std::string& name) const;

        // float uniforms
        void set_uniform(int location, float value);
        void set_uniform(int location, float x, float y);
        void set_uniform(int location, float x, float y, float z);
        void set_uniform(int location, float x, float y, float z, float w);
        void set_uniform_fv(int location, std::span<const float> values, int components = 1);

        // int uniforms
        void set_uniform(int location, int value);
        void set_uniform(int location, int x, int y);
        void set_uniform(int location, int x, int y, int z);
        void set_uniform(int location, int x, int y, int z, int w);

        // unsigned int uniforms
        void set_uniform(int location, unsigned int value);
        void set_uniform(int location, unsigned int x, unsigned int y);
        void set_uniform(int location, unsigned int x, unsigned int y, unsigned int z);
        void set_uniform(int location, unsigned int x, unsigned int y, unsigned int z, unsigned int w);

        // matrix uniforms
        void set_uniform_matrix2(int location, const float* data, bool transpose = false);
        void set_uniform_matrix3(int location, const float* data, bool transpose = false);
        void set_uniform_matrix4(int location, const float* data, bool transpose = false);
        void set_uniform_matrix2x3(int location, const float* data, bool transpose = false);
        void set_uniform_matrix3x2(int location, const float* data, bool transpose = false);
        void set_uniform_matrix2x4(int location, const float* data, bool transpose = false);
        void set_uniform_matrix4x2(int location, const float* data, bool transpose = false);
        void set_uniform_matrix3x4(int location, const float* data, bool transpose = false);
        void set_uniform_matrix4x3(int location, const float* data, bool transpose = false);

        // Uniform blocks
        [[nodiscard]] unsigned int uniform_block_index(const std::string& name) const;
        void set_uniform_block_binding(unsigned int block_index, unsigned int binding_point);

        [[nodiscard]] bool is_linked() const noexcept;
        [[nodiscard]] bool is_created() const noexcept;
        [[nodiscard]] unsigned int native_handle() const noexcept;
        [[nodiscard]] bool is_valid_for_current_generation() const noexcept;
        [[nodiscard]] std::uint64_t creation_generation() const noexcept;

        void reset_handle_no_gl() noexcept;

    private:
        unsigned int handle_ = 0;
        bool linked_ = false;
        std::vector<unsigned int> owned_shader_handles_;
        std::uint64_t generation_ = 0;
    };
}
