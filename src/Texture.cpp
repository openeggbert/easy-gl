#include "easygl/Texture.hpp"
#include <metagl/metagl.hpp>

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
        if (is_created()) return;
        metagl::glGenTextures(1, &handle_);
    }

    void Texture::destroy() noexcept
    {
        if (is_created())
        {
            metagl::glDeleteTextures(1, &handle_);
            handle_ = 0;
        }
    }

    void Texture::bind(TextureTarget /*target*/) const
    {
        metagl::glActiveTexture(metagl::TextureUnit::Texture0);
        metagl::glBindTexture(metagl::TextureTarget::Texture2D, handle_);
    }

    void Texture::set_image_2d(TextureTarget /*target*/, int level, int width, int height, const void* data)
    {
        metagl::glBindTexture(metagl::TextureTarget::Texture2D, handle_);

        metagl::glPixelStorei(metagl::PixelStoreParam::UnpackAlignment, 1);

        metagl::glTexImage2D(metagl::TextureTarget::Texture2D, level,
                             metagl::InternalFormat::Rgba,
                             width, height, 0,
                             metagl::PixelFormat::Rgba, metagl::PixelType::UnsignedByte, data);

        metagl::glTexParameteri(metagl::TextureTarget::Texture2D, metagl::TextureParameter::MinFilter, static_cast<GLint>(metagl::TextureFilter::Linear));
        metagl::glTexParameteri(metagl::TextureTarget::Texture2D, metagl::TextureParameter::MagFilter, static_cast<GLint>(metagl::TextureFilter::Linear));
        metagl::glTexParameteri(metagl::TextureTarget::Texture2D, metagl::TextureParameter::WrapS, static_cast<GLint>(metagl::TextureWrap::ClampToEdge));
        metagl::glTexParameteri(metagl::TextureTarget::Texture2D, metagl::TextureParameter::WrapT, static_cast<GLint>(metagl::TextureWrap::ClampToEdge));
    }

    bool Texture::is_created() const noexcept { return handle_ != 0; }
    unsigned int Texture::native_handle() const noexcept { return handle_; }
}
