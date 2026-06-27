#pragma once

#include <cstdint>
#include <span>
#include "easygl/Export.hpp"
#include "easygl/Types.hpp"
#include "easygl/detail/GenerationTracked.hpp"
#include "easygl/detail/NonCopyable.hpp"

namespace easygl
{
    class Texture;
    class Renderbuffer;
    class EASYGL_API Framebuffer : public detail::NonCopyable, public detail::GenerationTracked
    {
    public:
        Framebuffer();
        ~Framebuffer();

        Framebuffer(Framebuffer&& other) noexcept;
        Framebuffer& operator=(Framebuffer&& other) noexcept;

        void create();
        void destroy() noexcept;
        void bind(FramebufferTarget target = FramebufferTarget::Framebuffer) const;
        static void unbind(FramebufferTarget target = FramebufferTarget::Framebuffer);

        void attach_texture_2d(FramebufferTarget target, FramebufferAttachment attachment,
                                TextureTarget tex_target, const Texture& texture, int level);
        void attach_texture_layer(FramebufferTarget target, FramebufferAttachment attachment,
                                   const Texture& texture, int level, int layer);
        void attach_renderbuffer(FramebufferTarget target, FramebufferAttachment attachment,
                                  const Renderbuffer& renderbuffer);

        [[nodiscard]] bool is_valid_gl_object() const;

        [[nodiscard]] FramebufferStatus check_status(FramebufferTarget target = FramebufferTarget::Framebuffer) const;
        [[nodiscard]] bool is_complete(FramebufferTarget target = FramebufferTarget::Framebuffer) const;

        void set_draw_buffers(std::span<const DrawBuffer> buffers);
        void set_read_buffer(ReadBuffer src);

        void invalidate(FramebufferTarget target, std::span<const FramebufferAttachment> attachments);
        void invalidate_sub(FramebufferTarget target, std::span<const FramebufferAttachment> attachments,
                            int x, int y, int width, int height);

        static void blit(int src_x0, int src_y0, int src_x1, int src_y1,
                         int dst_x0, int dst_y0, int dst_x1, int dst_y1,
                         ClearBufferBit mask, BlitFilter filter);
        void blit_to(const Framebuffer& dst,
                     int src_x0, int src_y0, int src_x1, int src_y1,
                     int dst_x0, int dst_y0, int dst_x1, int dst_y1,
                     ClearBufferBit mask, BlitFilter filter) const;

        static Framebuffer create_color_depth(const Texture& color, const Renderbuffer& depth);

    };
}
