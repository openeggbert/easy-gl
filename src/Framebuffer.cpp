#include "easygl/Framebuffer.hpp"
#include "easygl/Texture.hpp"
#include "easygl/Renderbuffer.hpp"
#include <metagl/metagl.hpp>

namespace easygl
{
    Framebuffer::Framebuffer() = default;
    Framebuffer::~Framebuffer()
    {
        destroy();
    }

    Framebuffer::Framebuffer(Framebuffer&& other) noexcept
    {
        handle_ = other.handle_;
        generation_ = other.generation_;
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
        metagl::FramebufferId fid{};
        metagl::glGenFramebuffers(1, &fid);
        handle_ = fid.value;
        generation_ = metagl::GetContextGeneration();
    }

    void Framebuffer::destroy() noexcept
    {
        if (is_created())
        {
            metagl::FramebufferId fid{handle_};
            metagl::glDeleteFramebuffers(1, &fid);
            handle_ = 0;
        }
    }

    void Framebuffer::bind(FramebufferTarget target) const
    {
        metagl::glBindFramebuffer(target, metagl::FramebufferId{handle_});
    }

    void Framebuffer::unbind(FramebufferTarget target)
    {
        metagl::glBindFramebuffer(target, metagl::FramebufferId{0});
    }

    void Framebuffer::attach_texture_2d(FramebufferTarget target, FramebufferAttachment attachment,
                                         TextureTarget tex_target, const Texture& texture, int level)
    {
        metagl::glFramebufferTexture2D(target, attachment, tex_target,
                                        metagl::TextureId{texture.native_handle()}, level);
    }

    void Framebuffer::attach_texture_layer(FramebufferTarget target, FramebufferAttachment attachment,
                                            const Texture& texture, int level, int layer)
    {
        metagl::glFramebufferTextureLayer(target, attachment,
                                           metagl::TextureId{texture.native_handle()}, level, layer);
    }

    void Framebuffer::attach_renderbuffer(FramebufferTarget target, FramebufferAttachment attachment,
                                           const Renderbuffer& renderbuffer)
    {
        metagl::glFramebufferRenderbuffer(target, attachment,
                                          metagl::RenderbufferTarget::Renderbuffer,
                                          metagl::RenderbufferId{renderbuffer.native_handle()});
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

    Framebuffer Framebuffer::create_color_depth(const Texture& color, const Renderbuffer& depth)
    {
        Framebuffer fb;
        fb.create();
        fb.bind();
        fb.attach_texture_2d(FramebufferTarget::Framebuffer,
                              metagl::to_framebuffer_attachment(metagl::ColorAttachment::Color0),
                              TextureTarget::Texture2D, color, 0);
        fb.attach_renderbuffer(FramebufferTarget::Framebuffer,
                                FramebufferAttachment::Depth, depth);
        return fb;
    }

    void Framebuffer::blit(int src_x0, int src_y0, int src_x1, int src_y1,
                            int dst_x0, int dst_y0, int dst_x1, int dst_y1,
                            ClearBufferBit mask, BlitFilter filter)
    {
        metagl::glBlitFramebuffer(src_x0, src_y0, src_x1, src_y1,
                                   dst_x0, dst_y0, dst_x1, dst_y1,
                                   mask, filter);
    }

}
namespace easygl { bool Framebuffer::is_valid_gl_object() const { return is_created() && metagl::glIsFramebuffer(metagl::FramebufferId{handle_}); } }
