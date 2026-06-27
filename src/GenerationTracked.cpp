#include "easygl/detail/GenerationTracked.hpp"
#include <metagl/metagl.hpp>

namespace easygl::detail
{
    bool GenerationTracked::is_created() const noexcept
    {
        return handle_ != 0;
    }

    unsigned int GenerationTracked::native_handle() const noexcept
    {
        return handle_;
    }

    bool GenerationTracked::is_valid_for_current_generation() const noexcept
    {
        return handle_ != 0 && generation_ == metagl::GetContextGeneration();
    }

    std::uint64_t GenerationTracked::creation_generation() const noexcept
    {
        return generation_;
    }

    void GenerationTracked::reset_handle_no_gl() noexcept
    {
        handle_ = 0;
        generation_ = 0;
    }

} // namespace easygl::detail
