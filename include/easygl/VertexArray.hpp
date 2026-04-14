#pragma once

#include "easygl/Export.hpp"

namespace easygl
{
    class EASYGL_API VertexArray
    {
    public:
        VertexArray();
        ~VertexArray();

        void create();
        void destroy() noexcept;
        void bind() const;

        [[nodiscard]] unsigned int native_handle() const noexcept;

    private:
        unsigned int handle_ = 0;
    };
}