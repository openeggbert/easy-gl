#include "easygl/ProgramPipeline.hpp"
#include "easygl/Exception.hpp"
#include "easygl/Program.hpp"
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
    {
        handle_ = other.handle_;
        generation_ = other.generation_;
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
        // Program pipelines (separable shader objects) are an ES 3.1+ feature
        // with no WebGL equivalent at all - not even WebGL 2 (see
        // ProgramPipeline.hpp and TODO.md). Without this check
        // glGenProgramPipelines would be a null function pointer on such a
        // context, crashing on a null-pointer call instead of raising a clear
        // error.
        if (!metagl::IsFunctionAvailable("glGenProgramPipelines"))
            throw UnsupportedFeatureException("Program pipelines are not supported by the current context (e.g. never available on WebGL).");
        metagl::ProgramPipelineId ppid{};
        metagl::glGenProgramPipelines(1, &ppid);
        handle_ = ppid.value;
        generation_ = metagl::GetContextGeneration();
    }

    void ProgramPipeline::destroy() noexcept
    {
        if (is_created())
        {
            metagl::ProgramPipelineId ppid{handle_};
            metagl::glDeleteProgramPipelines(1, &ppid);
            handle_ = 0;
        }
    }

    void ProgramPipeline::bind() const
    {
        metagl::glBindProgramPipeline(metagl::ProgramPipelineId{handle_});
    }

    void ProgramPipeline::unbind()
    {
        metagl::glBindProgramPipeline(metagl::ProgramPipelineId{0});
    }

    void ProgramPipeline::use_stages(ShaderStageMask stages, const Program& program)
    {
        metagl::glUseProgramStages(metagl::ProgramPipelineId{handle_}, stages,
                                    metagl::ProgramId{program.native_handle()});
    }

    void ProgramPipeline::set_active_shader_program(const Program& program)
    {
        metagl::glActiveShaderProgram(metagl::ProgramPipelineId{handle_},
                                       metagl::ProgramId{program.native_handle()});
    }

    void ProgramPipeline::validate() const
    {
        metagl::glValidateProgramPipeline(metagl::ProgramPipelineId{handle_});
    }

    std::string ProgramPipeline::info_log() const
    {
        if (!is_created()) return "";

        int length = 0;
        metagl::glGetProgramPipelineiv(metagl::ProgramPipelineId{handle_}, metagl::ProgramPipelineParameter::InfoLogLength, &length);
        if (length <= 0) return "";

        std::vector<char> buffer(static_cast<std::size_t>(length));
        metagl::glGetProgramPipelineInfoLog(metagl::ProgramPipelineId{handle_}, length, nullptr, buffer.data());
        return std::string(buffer.data());
    }

}
namespace easygl { bool ProgramPipeline::is_valid_gl_object() const { return is_created() && metagl::glIsProgramPipeline(metagl::ProgramPipelineId{handle_}); } }
