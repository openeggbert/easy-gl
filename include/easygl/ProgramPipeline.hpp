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
