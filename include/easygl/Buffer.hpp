#pragma once

#include <cstddef>
#include "easygl/Export.hpp"

namespace easygl
{
    enum class BufferTarget
    {
        Array,
        ElementArray,
        Uniform,
        CopyRead,
        CopyWrite
    };

    class EASYGL_API Buffer
    {
    public:
        Buffer();
        ~Buffer();

        Buffer(const Buffer&) = delete;
        Buffer& operator=(const Buffer&) = delete;

        Buffer(Buffer&& other) noexcept;
        Buffer& operator=(Buffer&& other) noexcept;

        void create();
        void destroy() noexcept;
        void bind(BufferTarget target) const;
        void set_data(const void* data, std::size_t size_in_bytes);

        [[nodiscard]] bool is_created() const noexcept;
        [[nodiscard]] unsigned int native_handle() const noexcept;

    private:
        unsigned int handle_ = 0;
    };
}