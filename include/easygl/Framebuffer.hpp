#pragma once

#include <cstdint>
#include <span>
#include "easygl/Export.hpp"
#include "easygl/Types.hpp"
#include "easygl/detail/NonCopyable.hpp"

namespace easygl
{
    class EASYGL_API Framebuffer : public detail::NonCopyable
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
                                TextureTarget tex_target, unsigned int texture, int level);
        void attach_texture_layer(FramebufferTarget target, FramebufferAttachment attachment,
                                   unsigned int texture, int level, int layer);
        void attach_renderbuffer(FramebufferTarget target, FramebufferAttachment attachment,
                                  unsigned int renderbuffer);

        [[nodiscard]] FramebufferStatus check_status(FramebufferTarget target = FramebufferTarget::Framebuffer) const;
        [[nodiscard]] bool is_complete(FramebufferTarget target = FramebufferTarget::Framebuffer) const;

        void set_draw_buffers(std::span<const DrawBuffer> buffers);
        void set_read_buffer(ReadBuffer src);

        void invalidate(FramebufferTarget target, std::span<const FramebufferAttachment> attachments);

        static void blit(int src_x0, int src_y0, int src_x1, int src_y1,
                         int dst_x0, int dst_y0, int dst_x1, int dst_y1,
                         ClearBufferBit mask, TextureFilter filter);

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
