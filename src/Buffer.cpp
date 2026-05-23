#include "easygl/Buffer.hpp"
#include <metagl/metagl.hpp>

namespace easygl
{
    static metagl::BufferTarget to_meta(BufferTarget target)
    {
        switch (target)
        {
            case BufferTarget::Array:        return metagl::BufferTarget::Array;
            case BufferTarget::ElementArray: return metagl::BufferTarget::ElementArray;
            case BufferTarget::Uniform:      return metagl::BufferTarget::Uniform;
            case BufferTarget::CopyRead:     return metagl::BufferTarget::CopyRead;
            case BufferTarget::CopyWrite:    return metagl::BufferTarget::CopyWrite;
            default:                         return metagl::BufferTarget::Array;
        }
    }

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
        if (is_created()) return;
        metagl::glGenBuffers(1, &handle_);
    }

    void Buffer::destroy() noexcept
    {
        if (is_created())
        {
            metagl::glDeleteBuffers(1, &handle_);
            handle_ = 0;
        }
    }

    void Buffer::bind(BufferTarget target) const
    {
        metagl::glBindBuffer(to_meta(target), handle_);
    }

    void Buffer::bind_base(BufferTarget target, unsigned int index) const
    {
        metagl::glBindBufferBase(to_meta(target), index, handle_);
    }

    void Buffer::set_data(const void* data, std::size_t size_in_bytes)
    {
        set_data(BufferTarget::Array, data, size_in_bytes);
    }

    void Buffer::set_data(BufferTarget target, const void* data, std::size_t size_in_bytes)
    {
        const auto meta_target = to_meta(target);
        metagl::glBindBuffer(meta_target, handle_);
        metagl::glBufferData(meta_target, static_cast<metagl::GLsizeiptr>(size_in_bytes), data, metagl::BufferUsage::StaticDraw);
    }

    void Buffer::set_sub_data(const void* data, std::size_t size_in_bytes, std::size_t offset_in_bytes)
    {
        set_sub_data(BufferTarget::Array, data, size_in_bytes, offset_in_bytes);
    }

    void Buffer::set_sub_data(BufferTarget target, const void* data, std::size_t size_in_bytes, std::size_t offset_in_bytes)
    {
        const auto meta_target = to_meta(target);
        metagl::glBindBuffer(meta_target, handle_);
        metagl::glBufferSubData(meta_target,
                                static_cast<metagl::GLintptr>(offset_in_bytes),
                                static_cast<metagl::GLsizeiptr>(size_in_bytes),
                                data);
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
