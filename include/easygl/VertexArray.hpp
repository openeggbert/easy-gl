#pragma once
#include <cstddef>
#include "easygl/Export.hpp"
#include "easygl/Types.hpp"
#include "easygl/detail/NonCopyable.hpp"

namespace easygl
{
    class EASYGL_API VertexArray : public detail::NonCopyable
    {
    public:
        VertexArray();
        ~VertexArray();

        VertexArray(VertexArray&& other) noexcept;
        VertexArray& operator=(VertexArray&& other) noexcept;

        void create();
        void destroy() noexcept;
        void bind() const;
        void unbind() const;

        void set_attribute_pointer(unsigned int index, int size, DataType type, bool normalized, std::size_t stride, const void* pointer);
        void enable_attribute(unsigned int index);

        [[nodiscard]] bool is_created() const noexcept;
        [[nodiscard]] unsigned int native_handle() const noexcept;

    private:
        unsigned int handle_ = 0;
    };
}