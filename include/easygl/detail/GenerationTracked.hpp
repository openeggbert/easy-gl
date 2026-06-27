#pragma once

#include <cstdint>
#include "easygl/Export.hpp"

namespace easygl::detail
{
    class EASYGL_API GenerationTracked
    {
    public:
        [[nodiscard]] bool is_created() const noexcept;
        [[nodiscard]] unsigned int native_handle() const noexcept;
        [[nodiscard]] bool is_valid_for_current_generation() const noexcept;
        [[nodiscard]] std::uint64_t creation_generation() const noexcept;
        void reset_handle_no_gl() noexcept;

    protected:
        unsigned int handle_ = 0;
        std::uint64_t generation_ = 0;
    };

} // namespace easygl::detail
