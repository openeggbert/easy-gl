#include "easygl/TransformFeedback.hpp"
#include <metagl/metagl.hpp>

namespace easygl
{
    TransformFeedback::TransformFeedback() = default;
    TransformFeedback::~TransformFeedback()
    {
        destroy();
    }

    TransformFeedback::TransformFeedback(TransformFeedback&& other) noexcept
        : handle_(other.handle_)
        , generation_(other.generation_)
    {
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
        metagl::glGenTransformFeedbacks(1, &handle_);
        generation_ = metagl::GetContextGeneration();
    }

    void TransformFeedback::destroy() noexcept
    {
        if (is_created())
        {
            metagl::glDeleteTransformFeedbacks(1, &handle_);
            handle_ = 0;
        }
    }

    void TransformFeedback::bind() const
    {
        metagl::glBindTransformFeedback(metagl::TransformFeedbackTarget::TransformFeedback, handle_);
    }

    void TransformFeedback::unbind()
    {
        metagl::glBindTransformFeedback(metagl::TransformFeedbackTarget::TransformFeedback, 0);
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

    void TransformFeedback::set_varyings(unsigned int program,
                                          std::span<const char* const> varyings,
                                          TransformFeedbackBufferMode buffer_mode)
    {
        metagl::glTransformFeedbackVaryings(program,
                                             static_cast<GLsizei>(varyings.size()),
                                             varyings.data(), buffer_mode);
    }

    bool TransformFeedback::is_created() const noexcept { return handle_ != 0; }
    void TransformFeedback::reset_handle_no_gl() noexcept { handle_ = 0; generation_ = 0; }
    unsigned int TransformFeedback::native_handle() const noexcept { return handle_; }
    bool TransformFeedback::is_valid_for_current_generation() const noexcept
    {
        return handle_ != 0 && generation_ == metagl::GetContextGeneration();
    }
    std::uint64_t TransformFeedback::creation_generation() const noexcept { return generation_; }
}
