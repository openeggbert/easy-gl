#pragma once

#include <cstdint>
#include <string>
#include "easygl/Export.hpp"
#include "easygl/detail/NonCopyable.hpp"
#include <metagl/metagl.hpp>

namespace easygl
{
    using ShaderType = metagl::ShaderType;

    class EASYGL_API Shader : public detail::NonCopyable
    {
    public:
        explicit Shader(ShaderType type);
        ~Shader();

        Shader(Shader&& other) noexcept;
        Shader& operator=(Shader&& other) noexcept;

        void create();
        void destroy() noexcept;
        void compile_from_source(const std::string& source);
        [[nodiscard]] unsigned int release_native_handle() noexcept;

        [[nodiscard]] std::string info_log() const;
        [[nodiscard]] ShaderType shader_type() const noexcept;
        [[nodiscard]] bool is_compiled() const noexcept;
        [[nodiscard]] bool is_created() const noexcept;
        [[nodiscard]] unsigned int native_handle() const noexcept;
        [[nodiscard]] bool is_valid_for_current_generation() const noexcept;
        [[nodiscard]] std::uint64_t creation_generation() const noexcept;

    private:
        ShaderType type_;
        unsigned int handle_ = 0;
        bool compiled_ = false;
        std::uint64_t generation_ = 0;
    };
}
