#pragma once

#include <cstdint>
#include "easygl/Export.hpp"
#include "easygl/Types.hpp"
#include "easygl/detail/GenerationTracked.hpp"
#include "easygl/detail/NonCopyable.hpp"

namespace easygl
{
    class EASYGL_API Renderbuffer : public detail::NonCopyable, public detail::GenerationTracked
    {
    public:
        Renderbuffer();
        ~Renderbuffer();

        Renderbuffer(Renderbuffer&& other) noexcept;
        Renderbuffer& operator=(Renderbuffer&& other) noexcept;

        void create();
        void destroy() noexcept;
        void bind() const;
        static void unbind();

        void set_storage(InternalFormat internal_format, int width, int height);
        void set_storage_multisample(int samples, InternalFormat internal_format, int width, int height);

    };
}
