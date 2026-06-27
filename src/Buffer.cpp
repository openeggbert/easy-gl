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
    {
        handle_ = other.handle_;
        generation_ = other.generation_;
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
        metagl::BufferId bid{};
        metagl::glGenBuffers(1, &bid);
        handle_ = bid.value;
        generation_ = metagl::GetContextGeneration();
    }

    void Buffer::destroy() noexcept
    {
        if (is_created())
        {
            metagl::BufferId bid{handle_};
            metagl::glDeleteBuffers(1, &bid);
            handle_ = 0;
        }
    }

    void Buffer::bind(BufferTarget target) const
    {
        metagl::glBindBuffer(target, metagl::BufferId{handle_});
    }

    void Buffer::bind_base(BufferTarget target, unsigned int index) const
    {
        metagl::glBindBufferBase(target, index, metagl::BufferId{handle_});
    }

    void Buffer::bind_range(BufferTarget target, unsigned int index, std::ptrdiff_t offset, std::ptrdiff_t size) const
    {
        metagl::glBindBufferRange(target, index, metagl::BufferId{handle_}, offset, size);
    }

    void Buffer::set_data(const void* data, std::size_t size_in_bytes)
    {
        set_data(BufferTarget::Array, data, size_in_bytes, BufferUsage::StaticDraw);
    }

    void Buffer::set_data(BufferTarget target, const void* data, std::size_t size_in_bytes)
    {
        set_data(target, data, size_in_bytes, BufferUsage::StaticDraw);
    }

    void Buffer::set_data(BufferTarget target, const void* data, std::size_t size_in_bytes, BufferUsage usage)
    {
        metagl::glBindBuffer(target, metagl::BufferId{handle_});
        metagl::glBufferData(target, static_cast<std::ptrdiff_t>(size_in_bytes), data, usage);
    }

    void Buffer::set_sub_data(const void* data, std::size_t size_in_bytes, std::size_t offset_in_bytes)
    {
        set_sub_data(BufferTarget::Array, data, size_in_bytes, offset_in_bytes);
    }

    void Buffer::set_sub_data(BufferTarget target, const void* data, std::size_t size_in_bytes, std::size_t offset_in_bytes)
    {
        metagl::glBindBuffer(target, metagl::BufferId{handle_});
        metagl::glBufferSubData(target,
                                static_cast<std::ptrdiff_t>(offset_in_bytes),
                                static_cast<std::ptrdiff_t>(size_in_bytes),
                                data);
    }

    void* Buffer::map_range(BufferTarget target, std::ptrdiff_t offset, std::ptrdiff_t length, MapBufferAccessMask access)
    {
        metagl::glBindBuffer(target, metagl::BufferId{handle_});
        return metagl::glMapBufferRange(target, offset, length, access);
    }

    void Buffer::flush_mapped_range(BufferTarget target, std::ptrdiff_t offset, std::ptrdiff_t length)
    {
        metagl::glFlushMappedBufferRange(target, offset, length);
    }

    bool Buffer::unmap(BufferTarget target)
    {
        return metagl::glUnmapBuffer(target) != 0;
    }

    int Buffer::get_parameter(BufferTarget target, BufferParameter pname) const
    {
        int value = 0;
        metagl::glGetBufferParameteriv(target, pname, &value);
        return value;
    }

    i64 Buffer::get_parameter64(BufferTarget target, BufferParameter pname) const
    {
        GLint64 value = 0;
        metagl::glGetBufferParameteri64v(target, pname, &value);
        return static_cast<i64>(value);
    }

    void* Buffer::get_pointer(BufferTarget target) const
    {
        void* ptr = nullptr;
        metagl::glGetBufferPointerv(target, metagl::BufferPointerParameter::MapPointer, &ptr);
        return ptr;
    }

    void Buffer::copy_sub_data(BufferTarget read_target, BufferTarget write_target,
                                std::ptrdiff_t read_offset, std::ptrdiff_t write_offset,
                                std::ptrdiff_t size)
    {
        metagl::glCopyBufferSubData(read_target, write_target, read_offset, write_offset, size);
    }

}
namespace easygl { bool Buffer::is_valid_gl_object() const { return is_created() && metagl::glIsBuffer(metagl::BufferId{handle_}); } }
