#pragma once

#include <cstdint>
#include <span>
#include <string_view>
#include "easygl/Export.hpp"
#include "easygl/Types.hpp"
#include "easygl/detail/GenerationTracked.hpp"
#include "easygl/detail/NonCopyable.hpp"

namespace easygl
{
    class Program;

    class EASYGL_API TransformFeedback : public detail::NonCopyable, public detail::GenerationTracked
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

    };
}
