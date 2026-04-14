#pragma once

#include "easygl/Export.hpp"
#include "easygl/detail/NonCopyable.hpp"

namespace easygl
{
    enum class TextureTarget
    {
        Texture2D,
        Texture3D,
        CubeMap
    };

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

        [[nodiscard]] bool is_created() const noexcept;
        [[nodiscard]] unsigned int native_handle() const noexcept;

    private:
        unsigned int handle_ = 0;
    };
}