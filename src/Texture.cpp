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
        , generation_(other.generation_)
    {
        other.handle_ = 0;
        other.generation_ = 0;
    }

    Texture& Texture::operator=(Texture&& other) noexcept
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

    void Texture::create()
    {
        if (is_created()) return;
        metagl::glGenTextures(1, &handle_);
        generation_ = metagl::GetContextGeneration();
    }

    void Texture::destroy() noexcept
    {
        if (is_created())
        {
            metagl::glDeleteTextures(1, &handle_);
            handle_ = 0;
        }
    }

    void Texture::bind(TextureTarget target) const
    {
        metagl::glBindTexture(target, handle_);
    }

    void Texture::active_bind(TextureUnit unit, TextureTarget target) const
    {
        metagl::glActiveTexture(unit);
        metagl::glBindTexture(target, handle_);
    }

    void Texture::set_parameter(TextureTarget target, TextureParameter pname, int value)
    {
        metagl::glTexParameteri(target, pname, value);
    }

    void Texture::set_parameter(TextureTarget target, TextureParameter pname, float value)
    {
        metagl::glTexParameterf(target, pname, value);
    }

    void Texture::set_image_2d(TextureTarget target, int level, int width, int height, const void* data)
    {
        metagl::glBindTexture(target, handle_);
        metagl::glPixelStorei(metagl::PixelStoreParam::UnpackAlignment, 1);
        metagl::glTexImage2D(target, level,
                             metagl::InternalFormat::Rgba,
                             width, height, 0,
                             metagl::PixelFormat::Rgba, metagl::PixelType::UnsignedByte, data);
        metagl::glTexParameteri(target, metagl::TextureParameter::MinFilter, static_cast<GLint>(metagl::TextureFilter::Linear));
        metagl::glTexParameteri(target, metagl::TextureParameter::MagFilter, static_cast<GLint>(metagl::TextureFilter::Linear));
        metagl::glTexParameteri(target, metagl::TextureParameter::WrapS, static_cast<GLint>(metagl::TextureWrap::ClampToEdge));
        metagl::glTexParameteri(target, metagl::TextureParameter::WrapT, static_cast<GLint>(metagl::TextureWrap::ClampToEdge));
    }

    void Texture::set_image_2d(TextureTarget target, int level,
                                InternalFormat internal_format, int width, int height,
                                PixelFormat format, PixelType type, const void* data)
    {
        metagl::glTexImage2D(target, level, internal_format, width, height, 0, format, type, data);
    }

    void Texture::set_image_3d(TextureTarget target, int level,
                                InternalFormat internal_format, int width, int height, int depth,
                                PixelFormat format, PixelType type, const void* data)
    {
        metagl::glTexImage3D(target, level, internal_format, width, height, depth, 0, format, type, data);
    }

    void Texture::set_sub_image_2d(TextureTarget target, int level,
                                    int x, int y, int width, int height,
                                    PixelFormat format, PixelType type, const void* data)
    {
        metagl::glTexSubImage2D(target, level, x, y, width, height, format, type, data);
    }

    void Texture::set_sub_image_3d(TextureTarget target, int level,
                                    int x, int y, int z, int width, int height, int depth,
                                    PixelFormat format, PixelType type, const void* data)
    {
        metagl::glTexSubImage3D(target, level, x, y, z, width, height, depth, format, type, data);
    }

    void Texture::set_storage_2d(TextureTarget target, int levels, InternalFormat internal_format, int width, int height)
    {
        metagl::glTexStorage2D(target, levels, internal_format, width, height);
    }

    void Texture::set_storage_3d(TextureTarget target, int levels, InternalFormat internal_format, int width, int height, int depth)
    {
        metagl::glTexStorage3D(target, levels, internal_format, width, height, depth);
    }

    void Texture::set_storage_2d_multisample(TextureTarget target, int samples, InternalFormat internal_format,
                                              int width, int height, bool fixed_sample_locations)
    {
        metagl::glTexStorage2DMultisample(target, samples, internal_format, width, height,
                                          fixed_sample_locations ? 1 : 0);
    }

    void Texture::set_compressed_image_2d(TextureTarget target, int level,
                                           CompressedInternalFormat internal_format,
                                           int width, int height,
                                           std::size_t image_size, const void* data)
    {
        metagl::glCompressedTexImage2D(target, level, internal_format, width, height, 0,
                                       static_cast<GLsizei>(image_size), data);
    }

    void Texture::set_compressed_image_3d(TextureTarget target, int level,
                                           CompressedInternalFormat internal_format,
                                           int width, int height, int depth,
                                           std::size_t image_size, const void* data)
    {
        metagl::glCompressedTexImage3D(target, level, internal_format, width, height, depth, 0,
                                       static_cast<GLsizei>(image_size), data);
    }

    void Texture::generate_mipmap(TextureTarget target)
    {
        metagl::glGenerateMipmap(target);
    }

    void Texture::bind_image(unsigned int unit, int level, bool layered, int layer,
                              ImageAccess access, InternalFormat format)
    {
        metagl::glBindImageTexture(unit, handle_, level, layered ? 1 : 0, layer, access, format);
    }

    bool Texture::is_created() const noexcept { return handle_ != 0; }
    void Texture::reset_handle_no_gl() noexcept { handle_ = 0; generation_ = 0; }
    unsigned int Texture::native_handle() const noexcept { return handle_; }
    bool Texture::is_valid_for_current_generation() const noexcept
    {
        return handle_ != 0 && generation_ == metagl::GetContextGeneration();
    }
    std::uint64_t Texture::creation_generation() const noexcept { return generation_; }
}
