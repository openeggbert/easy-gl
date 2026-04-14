#pragma once

#include <string>
#include "easygl/Export.hpp"
#include "easygl/detail/NonCopyable.hpp"

namespace easygl
{
    enum class ShaderStage
    {
        Vertex,
        Fragment,
        Geometry,
        TessControl,
        TessEvaluation,
        Compute
    };

    class EASYGL_API Shader : public detail::NonCopyable
    {
    public:
        explicit Shader(ShaderStage stage);
        ~Shader();

        Shader(Shader&& other) noexcept;
        Shader& operator=(Shader&& other) noexcept;

        void create();
        void destroy() noexcept;
        void compile_from_source(const std::string& source);

        [[nodiscard]] std::string info_log() const;
        [[nodiscard]] ShaderStage stage() const noexcept;
        [[nodiscard]] bool is_compiled() const noexcept;
        [[nodiscard]] bool is_created() const noexcept;
        [[nodiscard]] unsigned int native_handle() const noexcept;

    private:
        ShaderStage stage_;
        unsigned int handle_ = 0;
        bool compiled_ = false;
    };
}