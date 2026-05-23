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
        metagl::glActiveTexture(0); // GL_TEXTURE0
        metagl::glBindTexture(metagl::TextureTarget::Texture2D, handle_);
    }

    void Texture::set_image_2d(TextureTarget /*target*/, int level, int width, int height, const void* data)
    {
        metagl::glBindTexture(metagl::TextureTarget::Texture2D, handle_);

        metagl::glPixelStorei(0x0CF5 /* GL_UNPACK_ALIGNMENT */, 1);

        metagl::glTexImage2D(metagl::TextureTarget::Texture2D, level,
                             static_cast<metagl::GLint>(0x1908) /* GL_RGBA */,
                             width, height, 0,
                             metagl::PixelFormat::Rgba, metagl::PixelType::UnsignedByte, data);

        metagl::glTexParameteriFilter(metagl::TextureTarget::Texture2D, metagl::TextureParameter::MinFilter, metagl::TextureFilter::Linear);
        metagl::glTexParameteriFilter(metagl::TextureTarget::Texture2D, metagl::TextureParameter::MagFilter, metagl::TextureFilter::Linear);
        metagl::glTexParameteriWrap(metagl::TextureTarget::Texture2D, metagl::TextureParameter::WrapS, metagl::TextureWrap::ClampToEdge);
        metagl::glTexParameteriWrap(metagl::TextureTarget::Texture2D, metagl::TextureParameter::WrapT, metagl::TextureWrap::ClampToEdge);
    }

    bool Texture::is_created() const noexcept { return handle_ != 0; }
    unsigned int Texture::native_handle() const noexcept { return handle_; }
}
