#include "easygl/VertexArray.hpp"

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
        // TODO: glGenVertexArrays / glCreateVertexArrays
    }

    void VertexArray::destroy() noexcept
    {
        // TODO: glDeleteVertexArrays
    }

    void VertexArray::bind() const
    {
        // TODO: glBindVertexArray
    }

    bool VertexArray::is_created() const noexcept { return handle_ != 0; }
    unsigned int VertexArray::native_handle() const noexcept { return handle_; }
}
