#include "easygl/VertexArray.hpp"
#include "platform/GlFunctions.hpp"

namespace easygl
{
    static platform::GLenum to_gl(DataType type)
    {
        switch (type)
        {
            case DataType::Float:         return platform::GL_FLOAT;
            case DataType::Byte:          return platform::GL_BYTE;
            case DataType::UnsignedByte:  return platform::GL_UNSIGNED_BYTE;
            case DataType::Short:         return platform::GL_SHORT;
            case DataType::UnsignedShort: return platform::GL_UNSIGNED_SHORT;
            case DataType::Int:           return platform::GL_INT;
            case DataType::UnsignedInt:   return platform::GL_UNSIGNED_INT;
            default: return 0;
        }
    }

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
        platform::g_gl.GenVertexArrays(1, &handle_);
    }

    void VertexArray::destroy() noexcept
    {
        if (is_created())
        {
            platform::g_gl.DeleteVertexArrays(1, &handle_);
            handle_ = 0;
        }
    }

    void VertexArray::bind() const
    {
        platform::g_gl.BindVertexArray(handle_);
    }

    void VertexArray::set_attribute_pointer(unsigned int index, int size, DataType type, bool normalized, std::size_t stride, const void* pointer)
    {
        platform::g_gl.VertexAttribPointer(index, size, to_gl(type), normalized ? 1 : 0, static_cast<platform::GLsizei>(stride), pointer);
    }

    void VertexArray::enable_attribute(unsigned int index)
    {
        platform::g_gl.EnableVertexAttribArray(index);
    }

    bool VertexArray::is_created() const noexcept { return handle_ != 0; }
    unsigned int VertexArray::native_handle() const noexcept { return handle_; }
}
