#include "easygl/Framebuffer.hpp"
#include <metagl/metagl.hpp>

namespace easygl
{
    Framebuffer::Framebuffer() = default;
    Framebuffer::~Framebuffer()
    {
        destroy();
    }

    Framebuffer::Framebuffer(Framebuffer&& other) noexcept
        : handle_(other.handle_)
        , generation_(other.generation_)
    {
        other.handle_ = 0;
        other.generation_ = 0;
    }

    Framebuffer& Framebuffer::operator=(Framebuffer&& other) noexcept
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

    void Framebuffer::create()
    {
        if (is_created()) return;
        metagl::glGenFramebuffers(1, &handle_);
        generation_ = metagl::GetContextGeneration();
    }

    void Framebuffer::destroy() noexcept
    {
        if (is_created())
        {
            metagl::glDeleteFramebuffers(1, &handle_);
            handle_ = 0;
        }
    }

    void Framebuffer::bind(FramebufferTarget target) const
    {
        metagl::glBindFramebuffer(target, handle_);
    }

    void Framebuffer::unbind(FramebufferTarget target)
    {
        metagl::glBindFramebuffer(target, 0);
    }

    void Framebuffer::attach_texture_2d(FramebufferTarget target, FramebufferAttachment attachment,
                                         TextureTarget tex_target, unsigned int texture, int level)
    {
        metagl::glFramebufferTexture2D(target, attachment, tex_target, texture, level);
    }

    void Framebuffer::attach_texture_layer(FramebufferTarget target, FramebufferAttachment attachment,
                                            unsigned int texture, int level, int layer)
    {
        metagl::glFramebufferTextureLayer(target, attachment, texture, level, layer);
    }

    void Framebuffer::attach_renderbuffer(FramebufferTarget target, FramebufferAttachment attachment,
                                           unsigned int renderbuffer)
    {
        metagl::glFramebufferRenderbuffer(target, attachment,
                                          metagl::RenderbufferTarget::Renderbuffer, renderbuffer);
    }

    FramebufferStatus Framebuffer::check_status(FramebufferTarget target) const
    {
        return metagl::glCheckFramebufferStatus(target);
    }

    bool Framebuffer::is_complete(FramebufferTarget target) const
    {
        return check_status(target) == metagl::FramebufferStatus::Complete;
    }

    void Framebuffer::set_draw_buffers(std::span<const DrawBuffer> buffers)
    {
        metagl::glDrawBuffers(static_cast<GLsizei>(buffers.size()), buffers.data());
    }

    void Framebuffer::set_read_buffer(ReadBuffer src)
    {
        metagl::glReadBuffer(src);
    }

    void Framebuffer::invalidate(FramebufferTarget target, std::span<const FramebufferAttachment> attachments)
    {
        metagl::glInvalidateFramebuffer(target, static_cast<GLsizei>(attachments.size()), attachments.data());
    }

    void Framebuffer::blit(int src_x0, int src_y0, int src_x1, int src_y1,
                            int dst_x0, int dst_y0, int dst_x1, int dst_y1,
                            ClearBufferBit mask, TextureFilter filter)
    {
        metagl::glBlitFramebuffer(src_x0, src_y0, src_x1, src_y1,
                                   dst_x0, dst_y0, dst_x1, dst_y1,
                                   mask, filter);
    }

    bool Framebuffer::is_created() const noexcept { return handle_ != 0; }
    void Framebuffer::reset_handle_no_gl() noexcept { handle_ = 0; generation_ = 0; }
    unsigned int Framebuffer::native_handle() const noexcept { return handle_; }
    bool Framebuffer::is_valid_for_current_generation() const noexcept
    {
        return handle_ != 0 && generation_ == metagl::GetContextGeneration();
    }
    std::uint64_t Framebuffer::creation_generation() const noexcept { return generation_; }
}
