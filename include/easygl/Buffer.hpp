#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include "easygl/Export.hpp"
#include "easygl/Types.hpp"
#include "easygl/detail/GenerationTracked.hpp"
#include "easygl/detail/NonCopyable.hpp"

namespace easygl
{
    class EASYGL_API Buffer : public detail::NonCopyable, public detail::GenerationTracked
    {
    public:
        Buffer();
        ~Buffer();

        Buffer(Buffer&& other) noexcept;
        Buffer& operator=(Buffer&& other) noexcept;

        void create();
        void destroy() noexcept;

        void bind(BufferTarget target) const;
        void bind_base(BufferTarget target, unsigned int index) const;
        void bind_range(BufferTarget target, unsigned int index, std::ptrdiff_t offset, std::ptrdiff_t size) const;

        void set_data(const void* data, std::size_t size_in_bytes);
        void set_data(BufferTarget target, const void* data, std::size_t size_in_bytes);
        void set_data(BufferTarget target, const void* data, std::size_t size_in_bytes, BufferUsage usage);

        template<typename T>
        void set_data(std::span<const T> data, BufferUsage usage = BufferUsage::StaticDraw)
        {
            set_data(data.data(), data.size_bytes(), usage);
        }

        template<typename T>
        void set_data(BufferTarget target, std::span<const T> data, BufferUsage usage = BufferUsage::StaticDraw)
        {
            set_data(target, data.data(), data.size_bytes(), usage);
        }

        void set_sub_data(const void* data, std::size_t size_in_bytes, std::size_t offset_in_bytes);
        void set_sub_data(BufferTarget target, const void* data, std::size_t size_in_bytes, std::size_t offset_in_bytes);

        template<typename T>
        void set_sub_data(std::span<const T> data, std::size_t offset_in_bytes = 0)
        {
            set_sub_data(data.data(), data.size_bytes(), offset_in_bytes);
        }

        template<typename T>
        void set_sub_data(BufferTarget target, std::span<const T> data, std::size_t offset_in_bytes = 0)
        {
            set_sub_data(target, data.data(), data.size_bytes(), offset_in_bytes);
        }

        [[nodiscard]] void* map_range(BufferTarget target, std::ptrdiff_t offset, std::ptrdiff_t length, MapBufferAccessMask access);
        void flush_mapped_range(BufferTarget target, std::ptrdiff_t offset, std::ptrdiff_t length);
        bool unmap(BufferTarget target);

        static void copy_sub_data(BufferTarget read_target, BufferTarget write_target,
                                   std::ptrdiff_t read_offset, std::ptrdiff_t write_offset,
                                   std::ptrdiff_t size);

    };
}
