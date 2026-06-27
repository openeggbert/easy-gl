#include "easygl/VertexArray.hpp"
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

}
namespace easygl { bool VertexArray::is_valid_gl_object() const { return is_created() && metagl::glIsVertexArray(metagl::VertexArrayId{handle_}); } }
