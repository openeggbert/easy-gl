#include "easygl/VertexArray.hpp"
#include "easygl/Buffer.hpp"
#include <metagl/metagl.hpp>

#include <cstdint>

namespace easygl
{
    VertexArray::VertexArray() = default;
    VertexArray::~VertexArray()
    {
        destroy();
    }

    VertexArray::VertexArray(VertexArray&& other) noexcept
    {
        handle_ = other.handle_;
        generation_ = other.generation_;
        other.handle_ = 0;
        other.generation_ = 0;
    }

    VertexArray& VertexArray::operator=(VertexArray&& other) noexcept
    {
        if (this != &other)
        {
            destroy();
            handle_ = other.handle_;
            generation_ = other.generation_;
            other.handle_ = 0;
            other.generation_ = 0;
        }
        return *this;
    }

    void VertexArray::create()
    {
        if (is_created()) return;
        metagl::VertexArrayId vaid{};
        metagl::glGenVertexArrays(1, &vaid);
        handle_ = vaid.value;
        generation_ = metagl::GetContextGeneration();
    }

    void VertexArray::destroy() noexcept
    {
        if (is_created())
        {
            metagl::VertexArrayId vaid{handle_};
            metagl::glDeleteVertexArrays(1, &vaid);
            handle_ = 0;
        }
    }

    void VertexArray::bind() const
    {
        metagl::glBindVertexArray(metagl::VertexArrayId{handle_});
    }

    void VertexArray::unbind() const
    {
        metagl::glBindVertexArray(metagl::VertexArrayId{0});
    }

    void VertexArray::set_attribute(const VertexAttribute& attribute)
    {
        set_attribute_pointer(
            attribute.index,
            attribute.components,
            attribute.type,
            attribute.normalized,
            attribute.stride_in_bytes,
            reinterpret_cast<const void*>(static_cast<std::uintptr_t>(attribute.offset_in_bytes)));

        if (attribute.enabled)
            enable_attribute(attribute.index);
    }

    void VertexArray::set_attribute_pointer(unsigned int index, int size, DataType type,
                                             bool normalized, std::size_t stride, const void* pointer)
    {
        metagl::glVertexAttribPointer(metagl::AttribLocation{index}, size, type, normalized ? 1 : 0,
                                      static_cast<metagl::GLsizei>(stride), pointer);
    }

    void VertexArray::set_attribute_i_pointer(unsigned int index, int size, DataType type,
                                               std::size_t stride, const void* pointer)
    {
        metagl::glVertexAttribIPointer(metagl::AttribLocation{index}, size, type,
                                       static_cast<metagl::GLsizei>(stride), pointer);
    }

    void VertexArray::enable_attribute(unsigned int index)
    {
        metagl::glEnableVertexAttribArray(metagl::AttribLocation{index});
    }

    void VertexArray::disable_attribute(unsigned int index)
    {
        metagl::glDisableVertexAttribArray(metagl::AttribLocation{index});
    }

    void VertexArray::set_attribute_divisor(unsigned int index, unsigned int divisor)
    {
        metagl::glVertexAttribDivisor(metagl::AttribLocation{index}, divisor);
    }

    void VertexArray::set_attribute_format(unsigned int attrib_index, int size, DataType type,
                                            bool normalized, unsigned int relative_offset)
    {
        metagl::glVertexAttribFormat(metagl::AttribLocation{attrib_index}, size, type,
                                      normalized ? GL_TRUE : GL_FALSE, relative_offset);
    }

    void VertexArray::set_attribute_i_format(unsigned int attrib_index, int size, DataType type,
                                              unsigned int relative_offset)
    {
        metagl::glVertexAttribIFormat(metagl::AttribLocation{attrib_index}, size, type, relative_offset);
    }

    void VertexArray::set_attribute_binding(unsigned int attrib_index, unsigned int binding_index)
    {
        metagl::glVertexAttribBinding(metagl::AttribLocation{attrib_index}, binding_index);
    }

    void VertexArray::bind_vertex_buffer(unsigned int binding_index, const Buffer& buffer,
                                          std::ptrdiff_t offset, std::size_t stride)
    {
        metagl::glBindVertexBuffer(binding_index, metagl::BufferId{buffer.native_handle()},
                                    static_cast<GLintptr>(offset), static_cast<GLsizei>(stride));
    }

    void VertexArray::set_binding_divisor(unsigned int binding_index, unsigned int divisor)
    {
        metagl::glVertexBindingDivisor(binding_index, divisor);
    }

}
namespace easygl { bool VertexArray::is_valid_gl_object() const { return is_created() && metagl::glIsVertexArray(metagl::VertexArrayId{handle_}); } }
