#include "easygl/Buffer.hpp"
#include <metagl/metagl.hpp>

namespace easygl
{
    Buffer::Buffer() = default;
    Buffer::~Buffer()
    {
        destroy();
    }

    Buffer::Buffer(Buffer&& other) noexcept
        : handle_(other.handle_)
        , generation_(other.generation_)
    {
        other.handle_ = 0;
        other.generation_ = 0;
    }

    Buffer& Buffer::operator=(Buffer&& other) noexcept
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

    void Buffer::create()
    {
        if (is_created()) return;
        metagl::glGenBuffers(1, &handle_);
        generation_ = metagl::GetContextGeneration();
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
        metagl::glBindBuffer(target, handle_);
    }

    void Buffer::bind_base(BufferTarget target, unsigned int index) const
    {
        metagl::glBindBufferBase(target, index, handle_);
    }

    void Buffer::set_data(const void* data, std::size_t size_in_bytes)
    {
        set_data(BufferTarget::Array, data, size_in_bytes);
    }

    void Buffer::set_data(BufferTarget target, const void* data, std::size_t size_in_bytes)
    {
        metagl::glBindBuffer(target, handle_);
        metagl::glBufferData(target, static_cast<std::ptrdiff_t>(size_in_bytes), data, metagl::BufferUsage::StaticDraw);
    }

    void Buffer::set_sub_data(const void* data, std::size_t size_in_bytes, std::size_t offset_in_bytes)
    {
        set_sub_data(BufferTarget::Array, data, size_in_bytes, offset_in_bytes);
    }

    void Buffer::set_sub_data(BufferTarget target, const void* data, std::size_t size_in_bytes, std::size_t offset_in_bytes)
    {
        metagl::glBindBuffer(target, handle_);
        metagl::glBufferSubData(target,
                                static_cast<std::ptrdiff_t>(offset_in_bytes),
                                static_cast<std::ptrdiff_t>(size_in_bytes),
                                data);
    }

    bool Buffer::is_created() const noexcept
    {
        return handle_ != 0;
    }
    void Buffer::reset_handle_no_gl() noexcept { handle_ = 0; generation_ = 0; }

    unsigned int Buffer::native_handle() const noexcept
    {
        return handle_;
    }
    bool Buffer::is_valid_for_current_generation() const noexcept
    {
        return handle_ != 0 && generation_ == metagl::GetContextGeneration();
    }
    std::uint64_t Buffer::creation_generation() const noexcept { return generation_; }
}
