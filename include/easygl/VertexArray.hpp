#pragma once
#include <cstddef>
#include <cstdint>
#include "easygl/Export.hpp"
#include "easygl/Types.hpp"
#include "easygl/detail/GenerationTracked.hpp"
#include "easygl/detail/NonCopyable.hpp"

namespace easygl
{
    class Buffer;

    struct EASYGL_API VertexAttribute
    {
        unsigned int index = 0;
        int components = 0;
        DataType type = DataType::Float;
        bool normalized = false;
        std::size_t stride_in_bytes = 0;
        std::size_t offset_in_bytes = 0;
        bool enabled = true;
    };

    class EASYGL_API VertexArray : public detail::NonCopyable, public detail::GenerationTracked
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

        [[nodiscard]] bool is_valid_gl_object() const;

        void set_attribute(const VertexAttribute& attribute);
        void set_attribute_pointer(unsigned int index, int size, DataType type,
                                   bool normalized, std::size_t stride, const void* pointer);
        void set_attribute_i_pointer(unsigned int index, int size, DataType type,
                                     std::size_t stride, const void* pointer);
        void enable_attribute(unsigned int index);
        void disable_attribute(unsigned int index);
        void set_attribute_divisor(unsigned int index, unsigned int divisor);

        // Separate vertex format (ES 3.1+)
        void set_attribute_format(unsigned int attrib_index, int size, DataType type,
                                   bool normalized, unsigned int relative_offset);
        void set_attribute_i_format(unsigned int attrib_index, int size, DataType type,
                                    unsigned int relative_offset);
        void set_attribute_binding(unsigned int attrib_index, unsigned int binding_index);
        void bind_vertex_buffer(unsigned int binding_index, const Buffer& buffer,
                                std::ptrdiff_t offset, std::size_t stride);
        void set_binding_divisor(unsigned int binding_index, unsigned int divisor);

    };
}
