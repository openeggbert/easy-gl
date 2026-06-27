#pragma once

#include <cstdint>
#include <string>
#include "easygl/Export.hpp"
#include "easygl/Types.hpp"
#include "easygl/detail/GenerationTracked.hpp"
#include "easygl/detail/NonCopyable.hpp"

namespace easygl
{

    class EASYGL_API Shader : public detail::NonCopyable, public detail::GenerationTracked
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

    private:
        ShaderType type_;
        bool compiled_ = false;
    };
}
