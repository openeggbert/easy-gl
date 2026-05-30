#pragma once

#include <cstdint>
#include "easygl/Export.hpp"
#include "easygl/Types.hpp"
#include "easygl/detail/NonCopyable.hpp"

namespace easygl
{
    class EASYGL_API Renderbuffer : public detail::NonCopyable
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

        [[nodiscard]] bool is_created() const noexcept;
        [[nodiscard]] unsigned int native_handle() const noexcept;
        [[nodiscard]] bool is_valid_for_current_generation() const noexcept;
        [[nodiscard]] std::uint64_t creation_generation() const noexcept;

        void reset_handle_no_gl() noexcept;

    private:
        unsigned int handle_ = 0;
        std::uint64_t generation_ = 0;
    };
}
