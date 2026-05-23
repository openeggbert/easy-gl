#pragma once

#include <cstddef>
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

    private:
        unsigned int handle_ = 0;
    };
}
