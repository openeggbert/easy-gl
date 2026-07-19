#pragma once

#include <cstdint>
#include <string>
#include "easygl/Export.hpp"
#include "easygl/Types.hpp"
#include "easygl/detail/GenerationTracked.hpp"
#include "easygl/detail/NonCopyable.hpp"

namespace easygl
{
    class Program;

    /// Separable shader program pipeline (ES 3.1+).
    ///
    /// @warning Permanently unavailable on WebGL, in both WebGL 1 and WebGL 2:
    ///          browser GL has no equivalent of separable shader objects, so
    ///          this is not a version/tier gap that a future WebGL version
    ///          could close. @ref create throws @ref UnsupportedFeatureException
    ///          instead of crashing when the underlying `glGenProgramPipelines`
    ///          entry point is unavailable (e.g. on any WebGL context).
    class EASYGL_API ProgramPipeline : public detail::NonCopyable, public detail::GenerationTracked
    {
    public:
        ProgramPipeline();
        ~ProgramPipeline();

        ProgramPipeline(ProgramPipeline&& other) noexcept;
        ProgramPipeline& operator=(ProgramPipeline&& other) noexcept;

        void create();
        void destroy() noexcept;
        void bind() const;
        static void unbind();

        void use_stages(ShaderStageMask stages, const Program& program);
        void set_active_shader_program(const Program& program);
        [[nodiscard]] bool is_valid_gl_object() const;
        void validate() const;

        [[nodiscard]] std::string info_log() const;

    };
}
