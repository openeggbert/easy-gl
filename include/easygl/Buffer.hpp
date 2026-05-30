#pragma once

#include <cstddef>
#include <cstdint>
#include "easygl/Export.hpp"
#include "easygl/detail/NonCopyable.hpp"
#include <metagl/metagl.hpp>

namespace easygl
{
    using BufferTarget = metagl::BufferTarget;

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
        void set_data(const void* data, std::size_t size_in_bytes);
        void set_data(BufferTarget target, const void* data, std::size_t size_in_bytes);
        void set_sub_data(const void* data, std::size_t size_in_bytes, std::size_t offset_in_bytes);
        void set_sub_data(BufferTarget target, const void* data, std::size_t size_in_bytes, std::size_t offset_in_bytes);

        [[nodiscard]] bool is_created() const noexcept;
        [[nodiscard]] unsigned int native_handle() const noexcept;
        [[nodiscard]] bool is_valid_for_current_generation() const noexcept;
        [[nodiscard]] std::uint64_t creation_generation() const noexcept;

        /// Zero the GL handle and generation without calling any gl* function.
        void reset_handle_no_gl() noexcept;

    private:
        unsigned int handle_ = 0;
        std::uint64_t generation_ = 0;
    };
}
