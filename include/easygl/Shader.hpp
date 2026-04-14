#pragma once

#include <string>
#include "easygl/Export.hpp"

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

    class EASYGL_API Shader
    {
    public:
        explicit Shader(ShaderStage stage);
        ~Shader();

        void compile_from_source(const std::string& source);

        [[nodiscard]] ShaderStage stage() const noexcept;
        [[nodiscard]] bool is_compiled() const noexcept;
        [[nodiscard]] unsigned int native_handle() const noexcept;

    private:
        ShaderStage stage_;
        unsigned int handle_ = 0;
        bool compiled_ = false;
    };
}