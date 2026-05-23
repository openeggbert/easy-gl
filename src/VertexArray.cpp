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
        : handle_(other.handle_)
    {
        other.handle_ = 0;
    }

    VertexArray& VertexArray::operator=(VertexArray&& other) noexcept
    {
        if (this != &other)
        {
            destroy();
            handle_ = other.handle_;
            other.handle_ = 0;
        }
        return *this;
    }

    void VertexArray::create()
    {
        if (is_created()) return;
        metagl::glGenVertexArrays(1, &handle_);
    }

    void VertexArray::destroy() noexcept
    {
        if (is_created())
        {
            metagl::glDeleteVertexArrays(1, &handle_);
            handle_ = 0;
        }
    }

    void VertexArray::bind() const
    {
        metagl::glBindVertexArray(handle_);
    }

    void VertexArray::unbind() const
    {
        metagl::glBindVertexArray(0);
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
        {
            enable_attribute(attribute.index);
        }
    }

    void VertexArray::set_attribute_pointer(unsigned int index, int size, DataType type, bool normalized, std::size_t stride, const void* pointer)
    {
        metagl::glVertexAttribPointer(index, size, type, normalized ? 1 : 0,
                                      static_cast<metagl::GLsizei>(stride), pointer);
    }

    void VertexArray::enable_attribute(unsigned int index)
    {
        metagl::glEnableVertexAttribArray(index);
    }

    bool VertexArray::is_created() const noexcept { return handle_ != 0; }
    unsigned int VertexArray::native_handle() const noexcept { return handle_; }
}
