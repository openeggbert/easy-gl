#include "easygl/Texture.hpp"

namespace easygl
{
    Texture::Texture() = default;
    Texture::~Texture()
    {
        destroy();
    }

    Texture::Texture(Texture&& other) noexcept
        : handle_(other.handle_)
    {
        other.handle_ = 0;
    }

    Texture& Texture::operator=(Texture&& other) noexcept
    {
        if (this != &other)
        {
            destroy();
            handle_ = other.handle_;
            other.handle_ = 0;
        }
        return *this;
    }

    void Texture::create()
    {
        // TODO: glGenTextures / glCreateTextures
    }

    void Texture::destroy() noexcept
    {
        // TODO: glDeleteTextures
    }

    void Texture::bind(TextureTarget) const
    {
        // TODO: glBindTexture
    }

    void Texture::set_image_2d(TextureTarget, int, int, int, const void*)
    {
        // TODO: glTexImage2D
    }

    bool Texture::is_created() const noexcept { return handle_ != 0; }
    unsigned int Texture::native_handle() const noexcept { return handle_; }
}
