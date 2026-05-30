#pragma once
#include <cstddef>
#include <cstdint>
#include "easygl/Export.hpp"
#include "easygl/Types.hpp"
#include "easygl/detail/NonCopyable.hpp"

namespace easygl
{
    struct EASYGL_API VertexAttribute
    {
        unsigned int index = 0;
        int components = 0;
        DataType type = DataType::Float;
        bool normalized = false;
        std::size_t stride_in_bytes = 0;
        std::size_t offset_in_bytes = 0;
        bool enabled = true;
    };

    class EASYGL_API VertexArray : public detail::NonCopyable
    {
    public:
        VertexArray();
        ~VertexArray();

        VertexArray(VertexArray&& other) noexcept;
        VertexArray& operator=(VertexArray&& other) noexcept;

        void create();
        void destroy() noexcept;
        void bind() const;
        void unbind() const;

        void set_attribute(const VertexAttribute& attribute);
        void set_attribute_pointer(unsigned int index, int size, DataType type, bool normalized, std::size_t stride, const void* pointer);
        void enable_attribute(unsigned int index);

        [[nodiscard]] bool is_created() const noexcept;
        [[nodiscard]] unsigned int native_handle() const noexcept;
        [[nodiscard]] bool is_valid_for_current_generation() const noexcept;
        [[nodiscard]] std::uint64_t creation_generation() const noexcept;

        /// Zero the GL handle and generation without calling any gl* function.
        void reset_handle_no_gl() noexcept;

    private:
        unsigned int handle_ = 0;
        std::uint64_t generation_ = 0;
    };
}