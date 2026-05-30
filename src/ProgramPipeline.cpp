#include "easygl/ProgramPipeline.hpp"
#include <metagl/metagl.hpp>
#include <vector>

namespace easygl
{
    ProgramPipeline::ProgramPipeline() = default;
    ProgramPipeline::~ProgramPipeline()
    {
        destroy();
    }

    ProgramPipeline::ProgramPipeline(ProgramPipeline&& other) noexcept
        : handle_(other.handle_)
        , generation_(other.generation_)
    {
        other.handle_ = 0;
        other.generation_ = 0;
    }

    ProgramPipeline& ProgramPipeline::operator=(ProgramPipeline&& other) noexcept
    {
        if (this != &other)
        {
            destroy();
            handle_ = other.handle_;
            generation_ = other.generation_;
            other.handle_ = 0;
            other.generation_ = 0;
        }
        return *this;
    }

    void ProgramPipeline::create()
    {
        if (is_created()) return;
        metagl::glGenProgramPipelines(1, &handle_);
        generation_ = metagl::GetContextGeneration();
    }

    void ProgramPipeline::destroy() noexcept
    {
        if (is_created())
        {
            metagl::glDeleteProgramPipelines(1, &handle_);
            handle_ = 0;
        }
    }

    void ProgramPipeline::bind() const
    {
        metagl::glBindProgramPipeline(handle_);
    }

    void ProgramPipeline::unbind()
    {
        metagl::glBindProgramPipeline(0);
    }

    void ProgramPipeline::use_stages(ShaderStageMask stages, unsigned int program)
    {
        metagl::glUseProgramStages(handle_, stages, program);
    }

    void ProgramPipeline::set_active_shader_program(unsigned int program)
    {
        metagl::glActiveShaderProgram(handle_, program);
    }

    void ProgramPipeline::validate() const
    {
        metagl::glValidateProgramPipeline(handle_);
    }

    std::string ProgramPipeline::info_log() const
    {
        if (!is_created()) return "";

        int length = 0;
        metagl::glGetProgramPipelineiv(handle_, metagl::ProgramPipelineParameter::InfoLogLength, &length);
        if (length <= 0) return "";

        std::vector<char> buffer(static_cast<std::size_t>(length));
        metagl::glGetProgramPipelineInfoLog(handle_, length, nullptr, buffer.data());
        return std::string(buffer.data());
    }

    bool ProgramPipeline::is_created() const noexcept { return handle_ != 0; }
    void ProgramPipeline::reset_handle_no_gl() noexcept { handle_ = 0; generation_ = 0; }
    unsigned int ProgramPipeline::native_handle() const noexcept { return handle_; }
    bool ProgramPipeline::is_valid_for_current_generation() const noexcept
    {
        return handle_ != 0 && generation_ == metagl::GetContextGeneration();
    }
    std::uint64_t ProgramPipeline::creation_generation() const noexcept { return generation_; }
}
