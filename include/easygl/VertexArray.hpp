#pragma once

#include "easygl/Export.hpp"
#include "easygl/detail/NonCopyable.hpp"

namespace easygl
{
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

        [[nodiscard]] bool is_created() const noexcept;
        [[nodiscard]] unsigned int native_handle() const noexcept;

    private:
        unsigned int handle_ = 0;
    };
}