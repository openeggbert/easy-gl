#pragma once

#include "easygl/Export.hpp"
#include "easygl/detail/NonCopyable.hpp"
#include <metagl/metagl.hpp>

namespace easygl
{
    using TextureTarget = metagl::TextureTarget;

    class EASYGL_API Texture : public detail::NonCopyable
    {
    public:
        Texture();
        ~Texture();

        Texture(Texture&& other) noexcept;
        Texture& operator=(Texture&& other) noexcept;

        void create();
        void destroy() noexcept;
        void bind(TextureTarget target) const;
        void set_image_2d(TextureTarget target, int level, int width, int height, const void* data);

        [[nodiscard]] bool is_created() const noexcept;
        [[nodiscard]] unsigned int native_handle() const noexcept;

    private:
        unsigned int handle_ = 0;
    };
}
