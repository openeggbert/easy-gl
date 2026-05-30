#pragma once

#include <cstdint>
#include <string>
#include "easygl/Export.hpp"
#include "easygl/Types.hpp"
#include "easygl/detail/NonCopyable.hpp"

namespace easygl
{
    class EASYGL_API ProgramPipeline : public detail::NonCopyable
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

        void use_stages(ShaderStageMask stages, unsigned int program);
        void set_active_shader_program(unsigned int program);
        void validate() const;

        [[nodiscard]] std::string info_log() const;

        [[nodiscard]] bool is_created() const noexcept;
        [[nodiscard]] unsigned int native_handle() const noexcept;
        [[nodiscard]] bool is_valid_for_current_generation() const noexcept;
        [[nodiscard]] std::uint64_t creation_generation() const noexcept;

        void reset_handle_no_gl() noexcept;

    private:
        unsigned int handle_ = 0;
        std::uint64_t generation_ = 0;
    };
}
