#pragma once

#include <cstdint>
#include "easygl/Export.hpp"
#include "easygl/Types.hpp"
#include "easygl/detail/NonCopyable.hpp"

namespace easygl
{
    class EASYGL_API Query : public detail::NonCopyable
    {
    public:
        Query();
        ~Query();

        Query(Query&& other) noexcept;
        Query& operator=(Query&& other) noexcept;

        void create();
        void destroy() noexcept;

        void begin(QueryTarget target) const;
        void end(QueryTarget target) const;

        [[nodiscard]] bool is_result_available() const;
        [[nodiscard]] unsigned int result() const;

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
