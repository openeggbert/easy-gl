#pragma once

#include "easygl/Export.hpp"
#include "easygl/Types.hpp"
#include "easygl/detail/NonCopyable.hpp"

#include <cstddef>
#include <cstdint>

namespace easygl
{
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
        void active_bind(TextureUnit unit, TextureTarget target) const;

        void set_parameter(TextureTarget target, TextureParameter pname, int value);
        void set_parameter(TextureTarget target, TextureParameter pname, float value);

        void set_image_2d(TextureTarget target, int level, int width, int height, const void* data);
        void set_image_2d(TextureTarget target, int level,
                          InternalFormat internal_format, int width, int height,
                          PixelFormat format, PixelType type, const void* data);
        void set_image_3d(TextureTarget target, int level,
                          InternalFormat internal_format, int width, int height, int depth,
                          PixelFormat format, PixelType type, const void* data);

        void set_sub_image_2d(TextureTarget target, int level,
                               int x, int y, int width, int height,
                               PixelFormat format, PixelType type, const void* data);
        void set_sub_image_3d(TextureTarget target, int level,
                               int x, int y, int z, int width, int height, int depth,
                               PixelFormat format, PixelType type, const void* data);

        void set_storage_2d(TextureTarget target, int levels, InternalFormat internal_format, int width, int height);
        void set_storage_3d(TextureTarget target, int levels, InternalFormat internal_format, int width, int height, int depth);
        void set_storage_2d_multisample(TextureTarget target, int samples, InternalFormat internal_format,
                                        int width, int height, bool fixed_sample_locations);

        void set_compressed_image_2d(TextureTarget target, int level,
                                     CompressedInternalFormat internal_format,
                                     int width, int height,
                                     std::size_t image_size, const void* data);
        void set_compressed_image_3d(TextureTarget target, int level,
                                     CompressedInternalFormat internal_format,
                                     int width, int height, int depth,
                                     std::size_t image_size, const void* data);

        void generate_mipmap(TextureTarget target);

        void bind_image(unsigned int unit, int level, bool layered, int layer,
                        ImageAccess access, InternalFormat format);

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
