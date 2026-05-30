#include "easygl/Renderbuffer.hpp"
#include <metagl/metagl.hpp>

namespace easygl
{
    Renderbuffer::Renderbuffer() = default;
    Renderbuffer::~Renderbuffer()
    {
        destroy();
    }

    Renderbuffer::Renderbuffer(Renderbuffer&& other) noexcept
        : handle_(other.handle_)
        , generation_(other.generation_)
    {
        other.handle_ = 0;
        other.generation_ = 0;
    }

    Renderbuffer& Renderbuffer::operator=(Renderbuffer&& other) noexcept
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

    void Renderbuffer::create()
    {
        if (is_created()) return;
        metagl::glGenRenderbuffers(1, &handle_);
        generation_ = metagl::GetContextGeneration();
    }

    void Renderbuffer::destroy() noexcept
    {
        if (is_created())
        {
            metagl::glDeleteRenderbuffers(1, &handle_);
            handle_ = 0;
        }
    }

    void Renderbuffer::bind() const
    {
        metagl::glBindRenderbuffer(metagl::RenderbufferTarget::Renderbuffer, handle_);
    }

    void Renderbuffer::unbind()
    {
        metagl::glBindRenderbuffer(metagl::RenderbufferTarget::Renderbuffer, 0);
    }

    void Renderbuffer::set_storage(InternalFormat internal_format, int width, int height)
    {
        metagl::glRenderbufferStorage(metagl::RenderbufferTarget::Renderbuffer,
                                      internal_format, width, height);
    }

    void Renderbuffer::set_storage_multisample(int samples, InternalFormat internal_format, int width, int height)
    {
        metagl::glRenderbufferStorageMultisample(metagl::RenderbufferTarget::Renderbuffer,
                                                  samples, internal_format, width, height);
    }

    bool Renderbuffer::is_created() const noexcept { return handle_ != 0; }
    void Renderbuffer::reset_handle_no_gl() noexcept { handle_ = 0; generation_ = 0; }
    unsigned int Renderbuffer::native_handle() const noexcept { return handle_; }
    bool Renderbuffer::is_valid_for_current_generation() const noexcept
    {
        return handle_ != 0 && generation_ == metagl::GetContextGeneration();
    }
    std::uint64_t Renderbuffer::creation_generation() const noexcept { return generation_; }
}
