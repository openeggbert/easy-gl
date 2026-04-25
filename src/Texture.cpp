#include "easygl/Texture.hpp"
#include "platform/GlFunctions.hpp"

namespace easygl
{
    static platform::GLenum to_gl(TextureTarget target)
    {
        switch (target)
        {
            case TextureTarget::Texture2D: return platform::GL_TEXTURE_2D;
            default: return 0;
        }
    }

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
        platform::g_gl.GenTextures(1, &handle_);
    }

    void Texture::destroy() noexcept
    {
        if (is_created())
        {
            platform::g_gl.DeleteTextures(1, &handle_);
            handle_ = 0;
        }
    }

    void Texture::bind(TextureTarget target) const
    {
        if (platform::g_gl.ActiveTexture != nullptr)
        {
            platform::g_gl.ActiveTexture(platform::GL_TEXTURE0);
        }

        platform::g_gl.BindTexture(to_gl(target), handle_);
    }

    void Texture::set_image_2d(TextureTarget target, int level, int width, int height, const void* data)
    {
        platform::GLenum gl_target = to_gl(target);
        platform::g_gl.BindTexture(gl_target, handle_);

        if (platform::g_gl.PixelStorei != nullptr)
        {
            platform::g_gl.PixelStorei(platform::GL_UNPACK_ALIGNMENT, 1);
        }

        platform::g_gl.TexImage2D(gl_target, level, platform::GL_RGBA, width, height, 0, platform::GL_RGBA, platform::GL_UNSIGNED_BYTE, data);
        
        platform::g_gl.TexParameteri(gl_target, platform::GL_TEXTURE_MIN_FILTER, platform::GL_LINEAR);
        platform::g_gl.TexParameteri(gl_target, platform::GL_TEXTURE_MAG_FILTER, platform::GL_LINEAR);
        platform::g_gl.TexParameteri(gl_target, platform::GL_TEXTURE_WRAP_S, platform::GL_CLAMP_TO_EDGE);
        platform::g_gl.TexParameteri(gl_target, platform::GL_TEXTURE_WRAP_T, platform::GL_CLAMP_TO_EDGE);
    }

    bool Texture::is_created() const noexcept { return handle_ != 0; }
    unsigned int Texture::native_handle() const noexcept { return handle_; }
}
