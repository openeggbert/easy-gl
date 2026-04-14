#pragma once

#include "easygl/Export.hpp"

namespace easygl
{
    enum class TextureTarget
    {
        Texture2D,
        Texture3D,
        CubeMap
    };

    class EASYGL_API Texture
    {
    public:
        Texture();
        ~Texture();

        void create();
        void destroy() noexcept;
        void bind(TextureTarget target) const;

        [[nodiscard]] unsigned int native_handle() const noexcept;

    private:
        unsigned int handle_ = 0;
    };
}