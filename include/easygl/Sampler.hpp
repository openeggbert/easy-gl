#pragma once

#include <cstdint>
#include "easygl/Export.hpp"
#include "easygl/Types.hpp"
#include "easygl/detail/NonCopyable.hpp"

namespace easygl
{
    class EASYGL_API Sampler : public detail::NonCopyable
    {
    public:
        Sampler();
        ~Sampler();

        Sampler(Sampler&& other) noexcept;
        Sampler& operator=(Sampler&& other) noexcept;

        void create();
        void destroy() noexcept;

        void bind(unsigned int unit) const;
        static void unbind(unsigned int unit);

        void set_parameter(TextureParameter pname, int value);
        void set_parameter(TextureParameter pname, float value);

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
