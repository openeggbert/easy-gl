#pragma once

#include <cstddef>
#include <cstdint>
#include "easygl/Export.hpp"
#include "easygl/Types.hpp"
#include "easygl/detail/NonCopyable.hpp"

namespace easygl
{
    class EASYGL_API Buffer : public detail::NonCopyable
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

        void set_sub_data(const void* data, std::size_t size_in_bytes, std::size_t offset_in_bytes);
        void set_sub_data(BufferTarget target, const void* data, std::size_t size_in_bytes, std::size_t offset_in_bytes);

        [[nodiscard]] void* map_range(BufferTarget target, std::ptrdiff_t offset, std::ptrdiff_t length, MapBufferAccessMask access);
        void flush_mapped_range(BufferTarget target, std::ptrdiff_t offset, std::ptrdiff_t length);
        bool unmap(BufferTarget target);

        static void copy_sub_data(BufferTarget read_target, BufferTarget write_target,
                                   std::ptrdiff_t read_offset, std::ptrdiff_t write_offset,
                                   std::ptrdiff_t size);

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
