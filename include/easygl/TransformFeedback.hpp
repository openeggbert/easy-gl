#pragma once

#include <cstdint>
#include <span>
#include <string_view>
#include "easygl/Export.hpp"
#include "easygl/Types.hpp"
#include "easygl/detail/NonCopyable.hpp"

namespace easygl
{
    class Program;

    class EASYGL_API TransformFeedback : public detail::NonCopyable
    {
    public:
        TransformFeedback();
        ~TransformFeedback();

        TransformFeedback(TransformFeedback&& other) noexcept;
        TransformFeedback& operator=(TransformFeedback&& other) noexcept;

        void create();
        void destroy() noexcept;
        void bind() const;
        static void unbind();

        static void begin(PrimitiveType primitive_mode);
        static void end();
        static void pause();
        static void resume();

        static void set_varyings(unsigned int program,
                                  std::span<const char* const> varyings,
                                  TransformFeedbackBufferMode buffer_mode);

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
