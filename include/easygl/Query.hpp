#pragma once

#include <cstdint>
#include "easygl/Export.hpp"
#include "easygl/Types.hpp"
#include "easygl/detail/GenerationTracked.hpp"
#include "easygl/detail/NonCopyable.hpp"

namespace easygl
{
    class EASYGL_API Query : public detail::NonCopyable, public detail::GenerationTracked
    {
    public:
        Query();
        ~Query();

        Query(Query&& other) noexcept;
        Query& operator=(Query&& other) noexcept;

        void create();
        void destroy() noexcept;

        [[nodiscard]] bool is_valid_gl_object() const;

        void begin(QueryTarget target) const;
        void end(QueryTarget target) const;

        [[nodiscard]] bool is_result_available() const;
        [[nodiscard]] unsigned int result() const;
        [[nodiscard]] u64 result_u64() const;

    };
}
