#pragma once

#include "easygl/Export.hpp"
#include "easygl/Types.hpp"
#include "easygl/detail/GenerationTracked.hpp"
#include "easygl/detail/NonCopyable.hpp"

#include <cstddef>
#include <cstdint>

namespace easygl
{
    class Buffer;

    class EASYGL_API Texture : public detail::NonCopyable, public detail::GenerationTracked
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
        void set_parameter_fv(TextureTarget target, TextureParameter pname, const float* values);
        void set_parameter_iv(TextureTarget target, TextureParameter pname, const int* values);
        void set_parameter_iiv(TextureTarget target, TextureParameter pname, const int* values);
        void set_parameter_iuiv(TextureTarget target, TextureParameter pname, const unsigned int* values);

        void get_parameter_fv(TextureTarget target, TextureParameter pname, float* out) const;
        void get_parameter_iv(TextureTarget target, TextureParameter pname, int* out) const;
        [[nodiscard]] int   get_level_parameter(TextureTarget target, int level, TextureLevelParameter pname) const;
        [[nodiscard]] float get_level_parameterf(TextureTarget target, int level, TextureLevelParameter pname) const;

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

        void set_buffer(TextureTarget target, InternalFormat internal_format, const Buffer& buffer);
        void set_buffer_range(TextureTarget target, InternalFormat internal_format,
                              const Buffer& buffer, std::ptrdiff_t offset, std::ptrdiff_t size);

        void copy_image_2d(TextureTarget target, int level, InternalFormat internal_format,
                           int x, int y, int width, int height);
        void copy_sub_image_2d(TextureTarget target, int level,
                               int xoff, int yoff, int x, int y, int width, int height);
        static void copy_image_sub_data(const Texture& src, TextureTarget src_target, int src_level,
                                         int sx, int sy, int sz,
                                         const Texture& dst, TextureTarget dst_target, int dst_level,
                                         int dx, int dy, int dz, int width, int height, int depth);

        [[nodiscard]] bool is_valid_gl_object() const;

        void bind_image(unsigned int unit, int level, bool layered, int layer,
                        ImageAccess access, InternalFormat format);

        static Texture create_2d(int width, int height,
                                  InternalFormat internal_format = InternalFormat::Rgba8,
                                  bool generate_mipmaps = false);

        static Texture create_2d_rgba8(int width, int height,
                                        const void* pixels,
                                        bool generate_mipmaps = false);

    };
}
