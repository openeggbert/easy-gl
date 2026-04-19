#include "easygl/Buffer.hpp"
#include "platform/GlFunctions.hpp"

namespace easygl
{
    static platform::GLenum to_gl(BufferTarget target)
    {
        switch (target)
        {
            case BufferTarget::Array:       return platform::GL_ARRAY_BUFFER;
            case BufferTarget::ElementArray: return platform::GL_ELEMENT_ARRAY_BUFFER;
            case BufferTarget::Uniform:      return platform::GL_UNIFORM_BUFFER;
            case BufferTarget::CopyRead:     return platform::GL_COPY_READ_BUFFER;
            case BufferTarget::CopyWrite:    return platform::GL_COPY_WRITE_BUFFER;
            default: return 0;
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
        platform::g_gl.GenBuffers(1, &handle_);
    }

    void Buffer::destroy() noexcept
    {
        if (is_created())
        {
            platform::g_gl.DeleteBuffers(1, &handle_);
            handle_ = 0;
        }
    }

    void Buffer::bind(BufferTarget target) const
    {
        platform::g_gl.BindBuffer(to_gl(target), handle_);
    }

    void Buffer::bind_base(BufferTarget target, unsigned int index) const
    {
        platform::g_gl.BindBufferBase(to_gl(target), index, handle_);
    }

    void Buffer::set_data(const void* data, std::size_t size_in_bytes)
    {
        set_data(BufferTarget::Array, data, size_in_bytes);
    }

    void Buffer::set_data(BufferTarget target, const void* data, std::size_t size_in_bytes)
    {
        const auto gl_target = to_gl(target);
        platform::g_gl.BindBuffer(gl_target, handle_);
        platform::g_gl.BufferData(gl_target, static_cast<platform::GLsizeiptr>(size_in_bytes), data, platform::GL_STATIC_DRAW);
    }

    void Buffer::set_sub_data(const void* data, std::size_t size_in_bytes, std::size_t offset_in_bytes)
    {
        set_sub_data(BufferTarget::Array, data, size_in_bytes, offset_in_bytes);
    }

    void Buffer::set_sub_data(BufferTarget target, const void* data, std::size_t size_in_bytes, std::size_t offset_in_bytes)
    {
        const auto gl_target = to_gl(target);
        platform::g_gl.BindBuffer(gl_target, handle_);
        platform::g_gl.BufferSubData(gl_target, static_cast<platform::GLintptr>(offset_in_bytes), static_cast<platform::GLsizeiptr>(size_in_bytes), data);
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