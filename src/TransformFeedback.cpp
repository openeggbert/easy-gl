#include "easygl/TransformFeedback.hpp"
#include "easygl/Program.hpp"
#include <metagl/metagl.hpp>

namespace easygl
{
    TransformFeedback::TransformFeedback() = default;
    TransformFeedback::~TransformFeedback()
    {
        destroy();
    }

    TransformFeedback::TransformFeedback(TransformFeedback&& other) noexcept
    {
        handle_ = other.handle_;
        generation_ = other.generation_;
        other.handle_ = 0;
        other.generation_ = 0;
    }

    TransformFeedback& TransformFeedback::operator=(TransformFeedback&& other) noexcept
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

    void TransformFeedback::create()
    {
        if (is_created()) return;
        metagl::TransformFeedbackId tfid{};
        metagl::glGenTransformFeedbacks(1, &tfid);
        handle_ = tfid.value;
        generation_ = metagl::GetContextGeneration();
    }

    void TransformFeedback::destroy() noexcept
    {
        if (is_created())
        {
            metagl::TransformFeedbackId tfid{handle_};
            metagl::glDeleteTransformFeedbacks(1, &tfid);
            handle_ = 0;
        }
    }

    void TransformFeedback::bind() const
    {
        metagl::glBindTransformFeedback(metagl::TransformFeedbackTarget::TransformFeedback, metagl::TransformFeedbackId{handle_});
    }

    void TransformFeedback::unbind()
    {
        metagl::glBindTransformFeedback(metagl::TransformFeedbackTarget::TransformFeedback, metagl::TransformFeedbackId{0});
    }

    void TransformFeedback::begin(PrimitiveType primitive_mode)
    {
        metagl::glBeginTransformFeedback(primitive_mode);
    }

    void TransformFeedback::end()
    {
        metagl::glEndTransformFeedback();
    }

    void TransformFeedback::pause()
    {
        metagl::glPauseTransformFeedback();
    }

    void TransformFeedback::resume()
    {
        metagl::glResumeTransformFeedback();
    }

    void TransformFeedback::set_varyings(const Program& program,
                                          std::span<const char* const> varyings,
                                          TransformFeedbackBufferMode buffer_mode)
    {
        metagl::glTransformFeedbackVaryings(metagl::ProgramId{program.native_handle()},
                                             static_cast<GLsizei>(varyings.size()),
                                             varyings.data(), buffer_mode);
    }

}
namespace easygl { bool TransformFeedback::is_valid_gl_object() const { return is_created() && metagl::glIsTransformFeedback(metagl::TransformFeedbackId{handle_}); } }
