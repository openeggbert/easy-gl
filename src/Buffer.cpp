#include "easygl/Buffer.hpp"

namespace easygl
{
    Buffer::Buffer() = default;
    Buffer::~Buffer()
    {
        destroy();
    }

    Buffer::Buffer(Buffer&& other) noexcept
        : handle_(other.handle_)
    {
        other.handle_ = 0;
    }

    Buffer& Buffer::operator=(Buffer&& other) noexcept
    {
        if (this != &other)
        {
            destroy();
            handle_ = other.handle_;
            other.handle_ = 0;
        }
        return *this;
    }

    void Buffer::create()
    {
        // TODO glGenBuffers / glCreateBuffers according to availability
    }

    void Buffer::destroy() noexcept
    {
        // TODO glDeleteBuffers
        handle_ = 0;
    }

    void Buffer::bind(BufferTarget) const
    {
        // TODO: glBindBuffer
    }

    void Buffer::bind_base(BufferTarget, unsigned int) const
    {
        // TODO: glBindBufferBase
    }

    void Buffer::set_data(const void*, std::size_t)
    {
        // TODO glBufferData / glNamedBufferData
    }

    bool Buffer::is_created() const noexcept
    {
        return handle_ != 0;
    }

    unsigned int Buffer::native_handle() const noexcept
    {
        return handle_;
    }
}